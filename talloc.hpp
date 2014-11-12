#include <cstdlib>
#include <string.h>

template <typename T>
void talloc(T *&ptr, unsigned size)
{
	ptr = static_cast<T *>(malloc(sizeof(T) * size));
}

template <typename T>
void tfree(T *&ptr)
{
	free(ptr);
	ptr = 0;
}

template <typename T>
void trealloc(T *&ptr, unsigned size)
{
	ptr = static_cast<T *>(realloc(ptr, sizeof(T) * size));
}

template <typename T>
void tmemset(T *ptr, int n, unsigned size)
{
	memset(ptr, n, size * sizeof(T));
}

template <typename T>
void tmemcpy(T *dst, T *src, unsigned size)
{
	memcpy(dst, src, size);
}
