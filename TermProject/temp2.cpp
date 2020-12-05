/*

#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<windows.h>

#define MAXBYTE 75

using namespace std;

struct Buffer {
	char buf[MAXBYTE + 1] = "";
};

vector<Buffer> lines;
int pageNumber = 1;

class Action {
public:
	virtual void doAction() = 0;
};

class NextPage : Action {
public:
	void doAction() override {
		// 허용 페이지가 넘을때 ? 예외처리 필요 
		if (lines.size() / 20 < pageNumber + 1) {
			cout << "page out of inbound" << endl;
			return;
		}

		++pageNumber;
	}
};

class PrevPage : Action {
public:
	void doAction() override {
		// 페이지 1보다 작을때 ? 예외처리 필요 
		if (pageNumber - 1 < 1) {
			cout << "page out of inbpound" << endl;
		}

		--pageNumber;
	}
};

class Insert : Action {
public:
	void doAction()
	{

	}
};

class Delete : Action {
public:
	void doAction() {

	}
};


void text_to_vector(string filename);
void clear_buffer(Buffer& buffer);
void show_interface();
void show_page(int page);
string get_command();


void main() {

	text_to_vector("test.txt");
	show_interface();

}

void show_interface() {
	string command;

	do {
		show_page(pageNumber);

		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "n:next page , p:previous page, i:insert, d:delete, c:change, s:search, t:save and quit " << endl;
		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "(consol message)" << endl;
		cout << "------------------------------------------------------------------------------------------" << endl;
		cout << "input:";
		command = get_command();
		cout << "------------------------------------------------------------------------------------------" << endl;

		Sleep(100);
		system("cls");

	} while (command.compare("t"));
}

string get_command() {
	string command;
	cin >> command;

	return command;
}

void show_page(int page) {
	int start = 20 * (page - 1) + 1;
	int end = start + 19;

	for (int i = start; i <= end; i++) {
		if (i < 10) {
			cout << " " << i << "| " << lines[i].buf << endl;
		}
		else {
			cout << i << "| " << lines[i].buf << endl;

		}
	}
}

void text_to_vector(string filename) {
	ifstream file;
	file.open(filename);
	string word, prev, sentance;
	int line = 1;
	int currentByte = 1;
	char buf[MAXBYTE] = "";
	Buffer buffer;

	lines.push_back(buffer);	// 줄 맞춤 편리성 위함

	word.clear();
	sentance.clear();
	prev.clear();

	while (file >> word) {
		sentance += word;

		if (word.at(word.length() - 1) == '.' && word.length() > 2) {
			// 문장의 끝을 만났을 때 벡터에 75byte 단위로 자른 후 push
			istringstream ss(sentance);

			while (getline(ss, word, ' ')) {
				if (!prev.empty()) {
					prev += ' ';
					for (int i = 0; i < prev.length(); i++) {
						buffer.buf[i] = prev.at(i);
					}
					currentByte += prev.length();
					prev.clear();
				}

				word += ' ';
				currentByte += word.length();

				if (currentByte / MAXBYTE >= 1) {
					prev = word;
					currentByte = 1;
					lines.push_back(buffer);
					clear_buffer(buffer);
					line++;
					word.clear();
					continue;
				}

				int  j = 0;

				for (int i = currentByte - word.length() - 1; i < currentByte - 1; i++) {
					buffer.buf[i] = word.at(j);
					j++;
				}
				word.clear();
				sentance.clear();
			}
		}
		else {
			sentance += ' ';
		}
		word.clear();
	}
	lines.push_back(buffer);
	file.close();
}

void clear_buffer(Buffer& buffer) {
	for (int i = 0; i <= MAXBYTE; i++) {
		buffer.buf[i] = '\0';
	}
}


*/