/*
 * Intvector.h
 * Georgios Pesmazoglou
 * Dynamic array of integers
 */

class IntVector
{
public:
        IntVector();
        IntVector(const IntVector &source);
        IntVector &operator=(const IntVector &rhs);
        ~IntVector();

        void destroy();

        /*
         * Return integer value at given index.
         *
         * Throws exception or abort with error message if index 
         * out of range.
         */
        int get(int index) const;

        /*
         * Set array element at given index to newVal.
         *
         * Throws exception or abort with error message if index 
         * out of range.
         */
        void set(int index, int newVal);

        /*
         * Return reference to array slot at given index.
         * This permits subscripted access to vector as 
         * L-value and/or R-value.
         *
         * Throws exception or abort with error message if index 
         * out of range.
         */
        int  &operator[](int index) const;

        /* Add newVal to end of vector, increasing current size by one. */
        void  add(int newVal);

        /* Return current size of vector. */
        int size() const;

        /* Return current capacity of vector */
        int capacity() const;

private:
      int *array;
      void expand();
      int used; /* just because its a smaller word */
      int cap; /* current_size */
};
