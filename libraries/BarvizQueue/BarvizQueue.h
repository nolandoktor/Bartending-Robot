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
    
    bool add ( T * );
    T * remove ();
    T * peek () const;
    T * removeMostRecent ();
    
    bool isEmpty () const;
    bool isFull () const;
    int size () const;
    
  private:
    typedef struct Node {
      T * item;
      Node * next;
      Node * prev;
    } Node;
    
    Node * head;
    Node * tail;

    Print * printer;
};

template <typename T> BarvizQueue<T> :: BarvizQueue ( const unsigned int maxCapacity) {
  this -> capacity = maxCapacity;
  this -> currSize = 0;
  this -> head = NULL;
  this -> tail = NULL;
}

template <typename T> BarvizQueue<T> :: ~BarvizQueue () {

  for (Node * ptr = this -> head; ptr != NULL; ptr = ptr -> next) {
    delete ptr;
  }

  this -> currSize = 0;
  this -> head = NULL;
  this -> tail = NULL;
}

template <typename T> bool BarvizQueue<T> :: add ( T * element ) {
  
  bool returnValue = false;

  if ( currSize < capacity ) {  
    
  	Node * ptr = new Node ();
  
  	ptr -> item = element;
  	ptr -> next = NULL;
  	ptr -> prev = NULL;
    
    if ( this -> head == NULL ) { // tail should be NULL too, in this case
      this -> head = this -> tail = ptr;
    }
    else {
      ptr -> prev = tail;
      this -> tail -> next = ptr;
      this -> tail = ptr;
    }
    
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

template <typename T> T * BarvizQueue<T> :: remove () {

  T * returnValue = NULL;
  
  if (!isEmpty ()) {

    returnValue = this -> head -> item;
    
    Node * ptr = this -> head;

    this -> head = this -> head -> next;
    delete ptr;
    
    if ( this -> head != NULL ) {
      this -> head -> prev = NULL;
    }
    else { // make tail NULL as well
      this -> tail = NULL;
    }
    
    this -> currSize --;
  }
  
  return returnValue;
}

template <typename T> T * BarvizQueue<T> :: peek () const {

  T * returnValue = NULL;

  if ( !isEmpty () ) {
    returnValue = this -> head -> item;
  }
  
  return returnValue;
}

template <typename T> T * BarvizQueue<T> :: removeMostRecent () {

  T * returnValue = NULL;
  
  if (!isEmpty ()) {

    returnValue = this -> tail -> item;
    
    Node * ptr = this -> tail;

    this -> tail = this -> tail -> prev;
    delete ptr;
    
    if ( this -> tail != NULL ) {
      this -> tail -> next = NULL;
    }
    else { // make head NULL as well
      this -> head = NULL;
    }
    
    this -> currSize --;
  }
  
  return returnValue;
}

#endif // _BARVIZ_QUEUE_H

