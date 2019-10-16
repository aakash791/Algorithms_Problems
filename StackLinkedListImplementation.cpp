#include<iostream>

using namespace std;

typedef struct List {

	int data;
	struct List* next; 

} Node;


class Stack {

	Node *node;
	
	public:
	
	Stack(){
		node = NULL;	
	} 
	
	~Stack(){
		delete[] node;
	}
	
	void Push(int data){
		if (node == NULL){
			node = new Node;
			node->data = data;
			node->next = NULL;
		}else{
			Node *temp1 = NULL;
			Node *temp = node; 
			while(temp->next!=NULL){
				temp = temp->next;
			}
			temp1 = new Node;
			temp1->data = data;
			temp1->next = NULL;
			temp->next = temp1;
		}
	}
	
	int Pop(){
		Node* temp = node;
		
		if (temp == NULL){
			cout << "Stack is empty" << endl;
			return -1;
		}
		
		if( temp->next == NULL ){
			int val1 = temp->data;
			node = NULL;
			delete temp;
			return val1;
		}
		
		Node* temp1 = node->next;
		while(temp1->next!=NULL){
			temp = temp->next;
			temp1 = temp1->next;
		}
	
		temp->next = NULL;
		int val = temp1->data;
		delete temp1;
		return val;
	}

	void print(){
		Node *temp = node;
		
		if (temp == NULL){
			cout << "LIST IS EMPTY" << endl;
			return;
		}
		 
		while (temp!=NULL){
			cout << temp->data << " ";
			temp = temp->next;
		}
		cout << endl;
	}
};

int main(){
	
	
	Stack s;
	
	s.Push(1);
	s.Push(2);
	
	s.print();
	
	cout << s.Pop() << endl;
	cout << s.Pop() << endl;
	
	s.print();
	
	s.Push(2);
	
	s.print();
		
	cout << s.Pop() << endl;
	cout << s.Pop() << endl;
	
	s.print();
	
	cout << endl << "********END***********" << endl;
 	
	return 0;
}
