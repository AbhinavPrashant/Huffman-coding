#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <cstdlib>

using namespace std;

struct decoderTreeNode {
	int data;
	struct decoderTreeNode *left;
	struct decoderTreeNode *right;
};

class Decoder{
	public:
		Decoder ();
		void decode (char *, string);
	private:
		struct decoderTreeNode *root;
		void buildTree (int, string);
		void decodeUsingTree (char *);
		void printTree (struct decoderTreeNode *);
};

Decoder::Decoder (){
	root = new decoderTreeNode;
	root->left = NULL;
	root->right = NULL;
	root->data = -1;
}

void Decoder::decode (char *encodedFile, string codeTable){

	ifstream eFile;
	ifstream cTable;

	cTable.open (codeTable);
	if (!cTable.is_open()){
		cout << "Failure ! codeTable file did not open" << endl;
		return;
	}

	string code;
	int number;
	while (cTable >> number >> code){

		buildTree (number, code);
	}

	decodeUsingTree (encodedFile);
}

void Decoder::decodeUsingTree (char *encodedFile){

//	FILE *efile;
	ifstream efile;
	ofstream decodedFile;
	char byteData;
	efile.open (encodedFile, ios::binary);
//        efile = fopen (encodedFile, "rb");

	decodedFile.open ("decoded.txt");

	if (! decodedFile.is_open()) {
		cout << "Failure! file could not open to write" << endl;
		return;
	}

	struct decoderTreeNode *temp = root;
//	unsigned char data;

//	while (fread (&byteData, 1, 1, efile)){
	while (efile.read (&byteData, 1)) {
		
		int j = 7;
		while (j >= 0){

			if ((byteData & (1 << j)) > 0){
				temp = temp->right;
			}
			else {
				temp = temp->left;
			}

			if (temp->data != -1){
				decodedFile << temp->data <<endl;
				temp = root;
			}

			j--;
		}
	}
}


void Decoder::buildTree (int data, string code){

	struct decoderTreeNode *temp = root;

	for (int i =0; i < code.size(); i++)
		if (code[i] == '1'){
			if (temp->right != NULL){
				temp = temp->right;
				continue;
			}
			temp->right = new decoderTreeNode;
			temp->right->right = NULL;
			temp->right->left = NULL;
			if (i == code.size()-1)
				temp->right->data = data;
			else
				temp->right->data = -1;

			temp = temp->right;
		}
		else {
			if (temp->left != NULL){
				temp = temp->left;
				continue;
			}
			temp->left = new decoderTreeNode;
			temp->left->left = NULL;
			temp->left->right = NULL;
			if (i == code.size()-1)
				temp->left->data = data;
	                else
	                        temp->left->data = -1;

	                temp = temp->left;
		}

}

int main (int argc, char *argv[]){

	if (argc != 3){
		cout << "Error! Encoded and code table file required" << endl;
		return -1;
	}

	clock_t startTime;
	startTime = clock();
	Decoder decoder;
		
	decoder.decode (argv[1], argv[2]);

	cout << "Total time to decode " << (clock()-startTime) << endl;
}
