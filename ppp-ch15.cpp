


#include<iostream>
#include <sstream>
#include<string>
#include<vector>
#include<span>
#include<stdexcept>
#include<random>
#include<stdint.h>
#include<list>
#include <map>
#include<unordered_map>
#include <set>
#include<memory>
#include <algorithm>
#include <type_traits>

template <typename T>
concept ContainerElementType = requires(T t) {
// Basic requirements (similar to semiregular)
	requires std::is_default_constructible<T>::value;  // Can be default-constructed
	requires std::is_copy_constructible<T>::value;    // Can be copy-constructed
	requires std::is_copy_assignable<T>::value;        // Can be copy-assigned

// Additional considerations for container elements
	requires std::is_destructible<T>::value;           // Can be destroyed (important for container management)

// Move semantics for efficiency
	requires std::is_move_constructible<T>::value;  // Can be move-constructed (for potential performance gains)
	requires std::is_move_assignable<T>::value;    // Can be move-assigned (for potential performance gains)
};


struct out_of_range {/*.....*/ };

int reasonable_size = std::numeric_limits<int>::max();

template<ContainerElementType T, typename A = std::allocator<T>>
struct Vector_rep {
	A alloc;			// allocator object of type std::allocator<T>
	int sz;
	T* elem;	
	int space;

	Vector_rep(const A& a, int n) : alloc{ a }, sz{ n }, elem{ alloc.allocate(n) }, space{ n } {}
	~Vector_rep() { alloc.deallocate(elem, space); }
};

template<ContainerElementType T, typename A = std::allocator<T>>
class Vector {
	/* invariant
	* 
	* if 0 <= n < sz, elem[n] is element n
	* sz <= space
	* if sz < space there is space for (space - sz) elements after elem[sz-1]
	* 
	*/

	Vector_rep<T, A> r;

public:
	Vector() : r{ A{}, 0 } {}								// default constructor

	Vector(const Vector&);									// copy constructor
	Vector(Vector&&);										// move constructor	

	Vector& operator=(const Vector&);						// copy assignment
	Vector& operator=(Vector&&);							// move assignment	

	explicit Vector(int sz); // constructor
	Vector(std::initializer_list<T> lst) : r{ A{}, (int)lst.size() } {             // initializer list constructor
		std::copy(lst.begin(), lst.end(), r.elem);
	}
	Vector& operator=(std::initializer_list<T> lst) {	// initializer list assignment
		if (lst.size() != r.sz) throw std::length_error{ "Vector::operator=()" };
		std::copy(lst.begin(), lst.end(), r.elem);
	}

	~Vector() {  } 							 // destructor

	int size() const { return r.sz; }
	int capacity() const { return r.space; }

	T& at(int n);
	const T& at(int n) const;

	T& operator[](int n) { return r.elem[n]; }			 // subscripting
	const T& operator[](int n) const { return r.elem[n]; } // subscripting const
	
	void reserve(int newalloc);						// increase capacity (without changing size)
	void resize(int newsize, T def = T{});						// increase size 	
	void push_back(T d);						// add element at end	

	T* begin() const { return r.elem; }			// iterator to first element
	T* end() const { return r.elem + r.sz; }       // iterator to one beyond the last element

};

template<ContainerElementType T, typename A = std::allocator<T>>
bool operator==(const Vector<T,A>& v1, const Vector<T,A>& v2) {
	if (v1.size() != v2.size()) return false;
	for (int i = 0; i < v1.size(); ++i) {
		if (v1[i] != v2[i]) return false;
	}
	return true;
};

template<ContainerElementType T, typename A = std::allocator<T>>
bool operator!=(const Vector<T,A>& v1, const Vector<T,A>& v2) {
	return !(v1 == v2);
};

template<ContainerElementType T, typename A >
Vector<T,A>::Vector(const Vector<T,A>& arg) {
	if (arg.size() <= size()) {
		std::move(arg.r.elem, arg.r.elem + arg.size(), r.elem);
		destroy(r.elem + arg.size(), r.elem + size());
	}
	auto tmp = arg;
	swap(*this, tmp);
}

template<ContainerElementType T, typename A >
Vector<T,A>::Vector(Vector<T,A>&& arg){
	swap(r, arg.r);
}

template<ContainerElementType T, typename A >
Vector<T, A>& Vector<T,A>::operator=(const Vector<T,A>& arg)
{
	if (arg.size() <= size()) {
		std::move(arg.r.elem, arg.r.elem + arg.size(), r.elem);
		destroy(r.elem + arg.size(), r.elem + size());
	}
	auto tmp = arg;
	swap(*this, tmp);
	return *this;

}

template<ContainerElementType T, typename A >
Vector<T, A>& Vector<T,A>::operator=(Vector<T,A>&& a)
{
	swap(r, a.r);
	return *this;
}

template<ContainerElementType T, typename A>
Vector<T, A>::Vector(int sz) : r{ A{}, sz }
{
	for (int i = 0; i < sz; ++i) r.elem[i] = 0;
}

template<ContainerElementType T, typename A>
T& Vector<T, A>::at(int n)
{
	if (n < 0 || r.sz <= n) throw out_of_range();
	return r.elem[n];
}

template<ContainerElementType T, typename A>
const T& Vector<T, A>::at(int n) const
{
	if (n < 0 || r.sz <= n) throw out_of_range();
	return r.elem[n];
}

template<ContainerElementType T, typename A >
void Vector<T,A>::reserve(int newalloc)
{
	if (newalloc <= r.space) return;
	Vector_rep<T, A> b{ r.alloc, newalloc };
	uninitialized_move(r.elem, r.elem + r.sz, b.elem);
	destroy(r.elem, r.elem + r.sz);
	swap(r, b);
}

template<ContainerElementType T, typename A >
void Vector<T,A>::resize(int newsize, T def)
{
	reserve(newsize);
	if (newsize > r.sz) uninitilized_fill(&r.elem[r.sz], &r.elem[newsize], def);
	if (newsize < r.sz) destroy(&r.elem[newsize], &r.elem[r.sz]);
	r.sz = newsize;
}

template<ContainerElementType T, typename A >
void Vector<T,A>::push_back(T d)
{
	reserve((r.space == 0) ? 8 : (2 * r.space)); // make sure that there is room for another element
	construct_at(&r.elem[r.sz], d); // construct a copy of d in elem[sz]
	++r.sz;
}

int main() {



}