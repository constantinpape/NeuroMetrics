#pragma once
#ifndef PYTHON_CONVERTER_HXX
#define PYTHON_CONVERTER_HXX

#include <type_traits>
#include <initializer_list>

#include <pybind11/pybind11.h>
#include <pybind11/cast.h>
#include <pybind11/numpy.h>

#include <andres/marray.hxx>

namespace py = pybind11;

namespace pybind11 {
    namespace detail {
        template<typename Type, size_t DIM, bool AUTO_CAST_TYPE>
        struct pymarray_caster;
    }
}


namespace andres
{

    template <typename VALUE_TYPE, size_t DIM = 0, bool AUTO_CAST_TYPE=true> 
    class PyView : public View<VALUE_TYPE, false>
    {
        friend struct pybind11::detail::pymarray_caster<VALUE_TYPE,DIM, AUTO_CAST_TYPE>;

      private:
        //pybind11::array_t<VALUE_TYPE> py_array;

                //pybind11::array_t<VALUE_TYPE> py_array;

        typename std::conditional<AUTO_CAST_TYPE,
                pybind11::array_t<VALUE_TYPE, py::array::forcecast>,
                pybind11::array_t<VALUE_TYPE, py::array::c_style > 
        >::type py_array;

      public:
        template <class ShapeIterator>
        PyView(pybind11::array_t<VALUE_TYPE> array, VALUE_TYPE *data, ShapeIterator begin, ShapeIterator end)
            : View<VALUE_TYPE, false>(begin, end, data, FirstMajorOrder, FirstMajorOrder), py_array(array)
        {
            auto info = py_array.request();
            VALUE_TYPE *ptr = (VALUE_TYPE *)info.ptr;

            std::vector<size_t> strides(info.strides.begin(),info.strides.end());
            for(size_t i=0; i<strides.size(); ++i){
                strides[i] /= sizeof(VALUE_TYPE);
            }
            this->assign( info.shape.begin(), info.shape.end(), strides.begin(), ptr, FirstMajorOrder);

        }

        PyView()
        {
        }
        const VALUE_TYPE & operator[](const uint64_t index)const{
            return this->operator()(index);
        }
        VALUE_TYPE & operator[](const uint64_t index){
            return this->operator()(index);
        }




        template <class ShapeIterator>
        PyView(ShapeIterator begin, ShapeIterator end)
        {
            this->assignFromShape(begin, end);
        }

        template <class ShapeIterator>
        void reshapeIfEmpty(ShapeIterator begin, ShapeIterator end){
            if(this->size() == 0){
                this->assignFromShape(begin, end);
            }
            else{
                auto c = 0;
                while(begin!=end){
                    if(this->shape(c)!=*begin){
                        throw std::runtime_error("given numpy array has an unusable shape");
                    }
                    ++begin;
                    ++c;
                }
            }
        }



    #ifdef HAVE_CPP11_INITIALIZER_LISTS
        template<class T_INIT>
        PyView(std::initializer_list<T_INIT> shape) : PyView(shape.begin(), shape.end())
        {
        }

        template<class T_INIT>
        void reshapeIfEmpty(std::initializer_list<T_INIT> shape) {
            this->reshapeIfEmpty(shape.begin(), shape.end());
        }
    #endif
    private:

        template <class ShapeIterator>
        void assignFromShape(ShapeIterator begin, ShapeIterator end)
        {
            std::vector<size_t> shape, strides;

            for (auto i = begin; i != end; ++i)
                shape.push_back(*i);

            for (size_t i = 0; i < shape.size(); ++i) {
                size_t stride = sizeof(VALUE_TYPE);
                for (size_t j = i + 1; j < shape.size(); ++j)
                    stride *= shape[j];
                strides.push_back(stride);
            }

            py_array = pybind11::array(pybind11::buffer_info(
                nullptr, sizeof(VALUE_TYPE), pybind11::format_descriptor<VALUE_TYPE>::value, shape.size(), shape, strides));
            pybind11::buffer_info info = py_array.request();
            VALUE_TYPE *ptr = (VALUE_TYPE *)info.ptr;

            for (size_t i = 0; i < shape.size(); ++i) {
                strides[i] /= sizeof(VALUE_TYPE);
            }
            this->assign(begin, end, strides.begin(), ptr, FirstMajorOrder);
        }

    };

}



namespace pybind11
{

    namespace detail
    {



        template <typename Type, size_t DIM, bool AUTO_CAST_TYPE> 
        struct pymarray_caster {
            typedef typename andres::PyView<Type, DIM, AUTO_CAST_TYPE> ViewType;
            typedef type_caster<typename intrinsic_type<Type>::type> value_conv;

            //typedef typename pybind11::array_t<Type, py::array::c_style > pyarray_type;

            typedef typename std::conditional<AUTO_CAST_TYPE,
                pybind11::array_t<Type, py::array::forcecast>,
                pybind11::array_t<Type, py::array::c_style > 
            >::type pyarray_type;

            typedef type_caster<pyarray_type> pyarray_conv;

            bool load(handle src, bool convert)
            {
                // convert numpy array to py::array_t
                pyarray_conv conv;
                if (!conv.load(src, convert)){
                    return false;
                }
                auto pyarray = (pyarray_type)conv;

                auto info = pyarray.request();

                if(DIM != 0 && DIM != info.shape.size()){
                    //std::cout<<"not matching\n";
                    return false;
                }
                Type *ptr = (Type *)info.ptr;

                ViewType result(pyarray, ptr, info.shape.begin(), info.shape.end());
                value = result;
                return true;
            }

            static handle cast(ViewType src, return_value_policy policy, handle parent)
            {
                pyarray_conv conv;
                return conv.cast(src.py_array, policy, parent);
            }

            PYBIND11_TYPE_CASTER(ViewType, _("array<") + value_conv::name + _(">"));
        };

        template <typename Type, size_t DIM, bool AUTO_CAST_TYPE>
        struct type_caster<andres::PyView<Type, DIM, AUTO_CAST_TYPE> >
            : pymarray_caster<Type,DIM, AUTO_CAST_TYPE> {
        };

    }
}


#endif  // PYTHON_CONVERTER_HXX
