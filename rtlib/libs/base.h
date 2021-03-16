#ifndef _RT_BASE_H_
#define _RT_BASE_H_

#define RT_API_WITH_RET(id, ret) \
{\
    __asm__ __volatile__("SSC $0, %1\r\nST $3, %0":"=m"(ret):"i"(id):);\
}

#define RT_API_NO_RET(id) \
{\
    __asm__ __volatile__("SSC $0, %0\r\n"::"i"(id):);\
}

#endif /* _RT_BASE_H_ */

