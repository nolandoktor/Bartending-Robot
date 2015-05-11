#ifndef _BARVIZ_QUEUE_H
#define _BARVIZ_QUEUE_H

// include Arduino basic header.
#include <Arduino.h>

template <typename T> class BarvizQueue {

  private:
    unsigned int capacity;
    int currSize;
  
  public:
    BarvizQueue ( const unsigned int );
    ~BarvizQueue ();
    
    bool add ( const T & );
    bool remove ( T & );
    bool peek ( T & ) const;
    bool removeMostRecent ( T & );
    int  asArray ( T *, unsigned int ) const;
    
    bool isEmpty () const;
    bool isFull () const;
    int size () const;
    
  private:
    T * array;
    int head;
    int tail;
};

template <typename T> BarvizQueue<T> :: BarvizQueue ( const unsigned int maxCapacity) {
  this -> capacity = maxCapacity;
  this -> currSize = 0;
  this -> array = new T [ this -> capacity ];
  this -> head = -1;
  this -> tail = -1;
}

template <typename T> BarvizQueue<T> :: ~BarvizQueue () {
  this -> currSize = 0;
}

template <typename T> bool BarvizQueue<T> :: add ( const T & element ) {
  
  bool returnValue = false;

  if ( currSize < capacity ) {  
    if ( this -> currSize == 0 ) {
      // First Element getting into the Queue, reset all indices
      this -> head = this -> tail = 0;
    }
    else {
      this -> tail = ( this -> tail + 1 ) % this -> capacity; 
    }
    
    this -> array [ this -> tail ] = element;
    this -> currSize ++;
    
    returnValue = true;
  }
  
  return returnValue;
}

template <typename T> bool BarvizQueue<T> :: isEmpty () const {
  return ( this -> currSize == 0 );
}

template <typename T> bool BarvizQueue<T> :: isFull () const {
  return ( this -> currSize == capacity );
}

template <typename T> int BarvizQueue<T> :: size () const {
  return this -> currSize;
}

template <typename T> bool BarvizQueue<T> :: remove ( T & removedElement ) {

  bool returnValue = false;
  
  if (!isEmpty ()) {
    T temp = this -> array [ this -> head ];
    this -> head = ( this -> head  + 1 ) % this -> capacity;
    this -> currSize --;
    returnValue = true;
    removedElement = temp;
  }
  
  return returnValue;
}

template <typename T> bool BarvizQueue<T> :: peek ( T & peekedElement ) const {

  bool returnValue = false;

  if ( !isEmpty () ) {
    peekedElement = this -> array [ this -> head ];
    returnValue = true;
  }
  
  return returnValue;
}

template <typename T> bool BarvizQueue<T> :: removeMostRecent ( T & removedElement ) {

  bool returnValue = false;
  
  if (!isEmpty ()) {
    T temp = this -> array [ this -> tail ];
    this -> tail = ( this -> tail + ( this -> capacity - 1 ) ) % this -> capacity;
    this -> currSize --;
    returnValue = true;
    removedElement = temp;
  }
  
  return returnValue;
}

template <typename T> int BarvizQueue<T> :: asArray ( T * destArray, unsigned int maxElements ) const {

  int copiedElements = 0;
  
  if ( !isEmpty () ) {
    int iter = this -> head;
    while ( ( copiedElements < maxElements ) && ( copiedElements < this -> currSize ) ) {
      destArray [ copiedElements ] = this -> array [ iter ];
      copiedElements ++;
      iter = ( iter + 1 ) % this -> capacity;
    }
  }
  
  return copiedElements;  
}

#endif // _BARVIZ_QUEUE_H

