#ifndef __FLIST_H__
#define __FLIST_H__

#include "defs.h"
#include "stdio.h"


/*

Slow list, but elements keep their ids forever
Is never shrunk.

Implementation in header because needed by compilers..

*/

template <class T>
class flist
{
    public:

        uint size, filled;
        T **elem; // array of element pointers

        flist()
        {
            size = 50; // standard size
            elem = new T*[size];

			for (uint i = 0; i < size; i++) elem[i] = NULL;

            filled = 0;
        }

        ~flist()
        {
            delete(elem);
        }

		T* at(uint i);
        uint add(T*);
		uint add_at(T*, uint pos);
		bool exists(uint i);
        bool remove(uint i);
		void clear();

    protected:
    private:
};


template <class T>
T* flist<T>::at(uint i)
{
	if (i < size && elem[i] != NULL) return elem[i];
	return NULL;
}

template <class T>
uint flist<T>::add(T *e)
{
    if (filled < size)
    {

        for (uint i = 0; i < size; i++)
        {

            if (elem[i] == NULL)
            {
                elem[i] = e;
                filled++;
                return i;
            }

        }

    }
    else
    {
		//std::cout << "Doubling flist size" << std::endl;
        // increase size
        T **newelem = new T*[size*2];

		uint i = 0;
        for (; i < size; i++)
        {
            newelem[i] = elem[i];
        }
		for (; i < size*2; i++) newelem[i] = NULL;

        size *= 2;

        delete[] elem;
        elem = newelem;
		if (elem[0] == NULL) std::cout << "ERROR in COPY222222!!!!!\n\n\n\n\n" << std::endl;

        // rekursive call
        return add(e);

    }

    // error, should never go here!
	std::cout << "ERROR in FLIST: more elements in list than possible" << std::endl;
    return 0;
}

template <class T>
uint flist<T>::add_at(T *e, uint pos)
{

    if (pos < size)
    {

        if (elem[pos] == NULL)
        {
            elem[pos] = e;
            filled++;
            return pos;
        }
		else
		{
			puts("ERROR: flist add_at at an already taken location");
		}

    }
    else
    {

        // increase size
        T **newelem = new T*[size*2];

		uint i = 0;
        for (; i < size; i++)
        {
            newelem[i] = elem[i];
        }
		for (; i < size*2; i++) newelem[i] = NULL;

        size *= 2;

        delete elem;
        elem = newelem;

        // rekursive call
        return add_at(e, pos);

    }

    // error, should never go here!
    return 0;

}

template <class T> bool flist<T>::exists(uint i)
{
    if (i >= size) return false;

    return (elem[i] != NULL);
}

template <class T> bool flist<T>::remove(uint i)
{
    if (i >= size) return false;

    elem[i] = NULL;
    filled--;

    return true;
}

template <class T> void flist<T>::clear()
{
	if (filled == 0) return;
	for (uint i = 0; i < size; i++) elem[i] = NULL;
	filled = 0;
}


#endif // __FLIST_H__
