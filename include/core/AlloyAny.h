/*
* Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
*
* Permission is hereby granted, free of charge, to Any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
//Kudos to Thomas Pigarelli (https://gist.github.com/Jefffrey/9202235) for reimplementing Boost's mysterious and wonderful "any" class
#ifndef ALLOYANY_H_
#define ALLOYANY_H_
#include <algorithm>
#include <typeinfo>
#include <exception>
#include <memory>
#include <type_traits>
namespace aly
{

	bool SANITY_CHECK_ANY();

	class Any;

	template<class Type> Type AnyCast(Any&);
	template<class Type> Type AnyCast(const Any&);
	template<class Type> Type* AnyCast(Any*);
	template<class Type> const Type* AnyCast(const Any*);
	struct BadAnyCast : public std::bad_cast {};

	class Any {
	public:

		template<class Type> friend
			Type AnyCast(Any&);

		template<class Type>
		friend Type AnyCast(const Any&);

		template<class Type>
		friend Type* AnyCast(Any*);

		template<class Type>
		friend const Type* AnyCast(const Any*);

		Any()
			: ptr(nullptr)
		{}

		Any(Any&& x)
			: ptr(std::move(x.ptr))
		{}

		Any(const Any& x) {
			if (x.ptr)
				ptr = x.ptr->clone();
		}

		template<class Type> Any(const Type& x)
			: ptr(new concrete<typename std::decay<const Type>::type>(x))
		{}

		Any& operator=(Any&& rhs) {
			ptr = std::move(rhs.ptr);
			return (*this);
		}

		Any& operator=(const Any& rhs) {
			ptr = std::move(Any(rhs).ptr);
			return (*this);
		}

		template<class T> Any& operator=(T&& x) {
			ptr.reset(new concrete<typename std::decay<T>::type>(typename std::decay<T>::type(x)));
			return (*this);
		}

		template<class T> Any& operator=(const T& x) {
			ptr.reset(new concrete<typename std::decay<T>::type>(typename std::decay<T>::type(x)));
			return (*this);
		}

		void clear() {
			ptr.reset(nullptr);
		}

		bool empty() const {
			return ptr == nullptr;
		}

		const std::type_info& type() const {
			return (!empty())
				? ptr->type()
				: typeid(void);
		}

	private:

		struct placeholder {

			virtual std::unique_ptr<placeholder> clone() const = 0;
			virtual const std::type_info& type() const = 0;
			virtual ~placeholder() {}

		};

		template<class T>
		struct concrete : public placeholder {

			concrete(T&& x)
				: value(std::move(x))
			{}

			concrete(const T& x)
				: value(x)
			{}

			virtual std::unique_ptr<placeholder> clone() const override {
				return std::unique_ptr<placeholder>(new concrete<T>(value));
			}

			virtual const std::type_info& type() const override {
				return typeid(T);
			}

			T value;

		};

		std::unique_ptr<placeholder> ptr;

	};

	template<class Type>
	Type AnyCast(Any& val) {
		if (val.ptr->type() != typeid(Type))
			throw BadAnyCast();
		return static_cast<Any::concrete<Type>*>(val.ptr.get())->value;
	}

	template<class Type>
	Type AnyCast(const Any& val) {
		return AnyCast<Type>(Any(val));
	}

	template<class Type>
	Type* AnyCast(Any* ptr) {
		return dynamic_cast<Type*>(ptr->ptr.get());
	}

	template<class Type>
	const Type* AnyCast(const Any* ptr) {
		return dynamic_cast<const Type*>(ptr->ptr.get());
	}
	class AnyInterface {
	public:
		template <typename T>
		void setValue(T value) {
			setValueImpl(Any(value));
		}
		template <typename T>
		T getValue() const {
			Any res = getValueImpl();
			return AnyCast<T>(res);
		}
	protected:
		virtual void setValueImpl(Any const & value) = 0;
		virtual Any getValueImpl() const = 0;
	};
	template<class T> class AnyValue : public AnyInterface {
	public:
		AnyValue(const T& value) :value(value) {
		}
		AnyValue() :value() {
		}
	protected:
		T value;
		virtual void setValueImpl(Any const & val) override {
			value = AnyCast<T>(val);
		}
		virtual Any getValueImpl() const override {
			return value;
		}
	};
}


#endif
