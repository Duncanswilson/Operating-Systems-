#include "QueueLinked.h"
 
/*Name:  QueueNode (Node constructor)
  Input Parameter:  the data for the node and a node pointer to the next item
  Return value: none
  Precondition: none
  Postcondition: a node is created
*/
template <typename DataType>
QueueLinked<DataType>::QueueNode::QueueNode(const DataType& nodeData, QueueNode* nextPtr)
   {
    dataItem = nodeData;;
    next = nextPtr;
   }
 
/*Name:  QueueLinked (Queue Contructor)
  Input Parameter: a max number (ignored)
  Return value: none
  Precondition: none
  Postcondition: creates queue, sets front and back to NULL
*/
template <typename DataType>
QueueLinked<DataType>::QueueLinked(int maxNumber)
   {
     front = NULL;
     back = NULL;
   }
 
/*Name: QueueLinked (Copy constructor)
  Input Parameter: the source queue
  Return value: none
  Precondition: if not empty
  Postcondition: the new node is creatred to match the parameterized node
*/
template <typename DataType>
QueueLinked<DataType>::QueueLinked(const QueueLinked& other)
   {
    if(!other.empty())
    {
     QueueNode *otherTemp, *destTemp;
     otherTemp = other.front;
     destTemp = front = new QueueNode(otherTemp->dataItem, NULL);
     otherTemp = otherTemp->next;
     while(otherTemp->next != NULL)
        {
         destTemp->next = new QueueNode(otherTemp->dataItem, NULL);
         otherTemp = otherTemp->next;
        }
      destTemp->dataItem = otherTemp->dataItem;
      destTemp->next = NULL;
    }
   }
 
/*Name:  operator = (overloaded equals opperator)
  Input Parameter: the source node for which the user wants assigned
  Return value: this node, for the sake of chaining
  Precondition: if this queue isn't at the same address of the source
                and it isn't empty 
  Postcondition: the source parameterized queue is copied into this queue, this queue is returned
*/
template <typename DataType>
QueueLinked<DataType>& QueueLinked<DataType>:: operator=(const QueueLinked& other)
   {
     while(this != &other)
       {
       if(!other.isEmpty())
         {
          QueueNode srcTemp, destTemp;
          clear();
          srcTemp = other.front;
          destTemp = front = new QueueNode(srcTemp->dataItem, NULL);
          srcTemp = srcTemp->next;
          while(srcTemp->next != NULL)
            {
             destTemp->next = new QueueNode(srcTemp->dataItem, NULL);
             srcTemp = srcTemp->next;            
            }
          destTemp->dataItem = srcTemp->dataItem;
          destTemp->next = NULL;
         }  
        }
       return *this;
   }
  
/*Name:  ~QueueLinked (Queue destuctor)
  Input Parameter: none
  Return value: none
  Precondition: none
  Postcondition: clears and deletes the queue
*/
template <typename DataType>
QueueLinked<DataType>::~QueueLinked()
   {
     clear();
   }
 
/*Name:  enqueue
  Input Parameter: newDataItem; a templated data item
  Return value: none
  Precondition: if not full
  Postcondition: the parameterized data is stored in the queue
*/
template <typename DataType>
void QueueLinked<DataType>::enqueue(const DataType& newDataItem) throw (logic_error)
   {
    if(!isFull())
      {
       if(isEmpty())
         {
           front = back = new QueueNode(newDataItem, NULL);
         }
       else
         {
          back = back->next = new QueueNode(newDataItem, NULL);
         }
      }
   }
 
/*Name:  dequeue
  Input Parameter: none 
  Return value: the front item from the queue
  Precondition: if not empty
  Postcondition: the front item from the queue is removed and returned
*/
template <typename DataType>
DataType QueueLinked<DataType>::dequeue() throw (logic_error)
   {
    if(!isEmpty())
      {
       if(front == back)
         {
           DataType temp = front->dataItem;
           delete front;
           front = back = NULL;
           return temp;
         }
       else
         {
          DataType temp = front->dataItem;
          QueueNode *t = front->next;
          delete front;
          front = t;
          return temp;
         }
      }
    throw logic_error("dequeue() while queue empty");
   }
 
/*Name: clear
  Input Parameter: none 
  Return value: none
  Precondition: if not empty
  Postcondition: delete each node in the queue, set pointers to NULL
*/
template <typename DataType>
void QueueLinked<DataType>::clear()
   {
     if(!isEmpty())
       {
        QueueNode *n;
        while(front != NULL)
           {
            n = front->next;
            delete front;
            front = n;
           }
        back = front;
       }
   }
 
/*Name: isEmpty
  Input Parameter: none
  Return value: boolean value representing the status of the queue
  Precondition: if front & back both equal NULL
  Postcondition: return true
*/
template <typename DataType>
bool QueueLinked<DataType>::isEmpty() const
   {
     if(front == NULL && back == NULL)
       {
        return true;
       }
      return false;
   }
 
/*Name:  isFull
  Input Parameter: none
  Return value: boolean value representing the status of the queue
  Precondition: none
  Postcondition: return false
*/
template <typename DataType>
bool QueueLinked<DataType>::isFull() const
   {
     return false;
   }
 
/*Name:  putFront
  Input Parameter:  a newDataItem to be placed in the front of the queue
  Return value: none
  Precondition: if not full
  Postcondition: create a new node with newDataItem and link it to the front
                 move front pointer
*/
template <typename DataType>
void QueueLinked<DataType>::putFront(const DataType& newDataItem) throw (logic_error)
   {
    if(!isFull())
      {
       if(front == back)
        {
         front = new QueueNode(newDataItem, back);
        }
       else
        {
         QueueNode *temp = new  QueueNode(newDataItem, front);
         front = temp;
        }
      }
   }
 
/*Name:  getRear
  Input Parameter: none
  Return value: the rear value
  Precondition: if not Empty
  Postcondition: removes and returns the data in rear
*/
template <typename DataType>
DataType QueueLinked<DataType>::getRear() throw (logic_error)
   {
    if(!isEmpty())
      {     
       DataType tempData = back-> dataItem;
       if(front == back)
         {
          delete back;
          front = back = NULL;
          return tempData;
         }    
       QueueNode *temp = front;
       while(temp->next != back)
         {
          temp = temp->next;
         } 
       delete temp->next;   
       back = temp;
       temp->next = NULL;
       return tempData;
      }
    throw logic_error("getRear() while queue empty");
   }
 
/*Name:  getLength
  Input Parameter: none
  Return value: an int value of the size
  Precondition: none
  Postcondition: returns the size of the queue
*/
template<typename DataType>
int QueueLinked<DataType>::getLength() const
   {
     if(!isEmpty())
       {
        int ctr = 0;
        QueueNode *t = new QueueNode(ctr,front->next);
        while(t != NULL)
          {
           ctr++;
           t = t->next;
          }
         return ctr;
       }
     return 0;
   }
 
/*Name:  showStructure
  Input Parameter: none
  Return value: none
  Precondition: none
  Postcondition: displays a message representative of what is currently in the queue
*/
template <typename DataType>
void QueueLinked<DataType>:: showStructure () const
 
// Linked list implementation. Outputs the elements in a queue. If
// the queue is empty, outputs "Empty queue". This operation is
// intended for testing and debugging purposes only.
 
{
    // Iterates through the queue
    QueueNode* p;
 
    if ( isEmpty() )
    cout << "Empty queue" << endl;
    else
    {
    cout << "Front\t";
    for ( p = front ; p != NULL ; p = p->next )
    {
        if( p == front )
        {
        cout << '[' << p->dataItem << "] ";
        }
        else
        {
        cout << p->dataItem << " ";
        }
    }
    cout << "\trear" << endl;
    }
}