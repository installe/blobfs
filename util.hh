#ifndef _UTIL_HH_
#define _UTIL_HH_

template<class C> C max(C x, C y);

template<class C> C max(C x, C y) {
     if (x > y)
	  return x;

     return y;
}

#endif /* _UTIL_HH_ */
