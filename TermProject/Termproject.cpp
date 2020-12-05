#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<windows.h>
#include<sstream>

#define MAXBYTE 75

using namespace std;

struct Buffer {
	char buf[152] = "";
};

vector<Buffer> lines;
int pageNumber = 1;
string consolMessage ="";


class Action {
public:
	virtual void doAction() = 0;
};

class NextPage :public Action {
public:
	void doAction() override {
		// 허용 페이지가 넘을때 ? 예외처리 필요 
		if (lines.size() / 20 < pageNumber ) {
			consolMessage = "error: page out of inbound";
			return;
		}
		++pageNumber;
	}
};

class PrevPage : public Action {
public:
	void doAction() override {
		// 페이지 1보다 작을때 ? 예외처리 필요 
		if (pageNumber - 1 < 1) {
			consolMessage = "error: page out of inbound";
			return;
		}
		--pageNumber;
	}
};

class Insert : public Action {
public:
	void i(int a, int b, string s) {
		row = a;
		col = b;
		word = s;
		doAction();
	}
	void doAction() override{
		
	}
public:
	int row, col;
	string word;
};

class Delete : public Action {
public:
	void d(int a, int b) {
		row = a;
		col = b;
	}
	void doAction() {

	}

public:
	int row, col;
};

class Change : public Action {
public:
	void c(string oldWord_, string newWord_) {
		oldWord = oldWord_;
		newWord = newWord_;
	}
	void doAction() {

	}
public:
	string oldWord, newWord;
};

class Search : public Action {
public:
	void s(string s_) {
		cout << s_ << endl;
		change = s_;
	}

	void doAction() {

	}
public:
	string change;
};

class SaveQuit : public Action {
public:
	void doAction() {

	}
};

class Term {
protected:
	Action* action;
	string filename;

public:
	Term(string filename_) : filename(filename_) {
		text_to_vector();
	}
	
	void show_page(int page) {
		int start = 20 * (page - 1) + 1;
		int end = start + 19;

		if (end > lines.size()) {
			end = lines.size()-1;
		}

		int lineNum;
		
		for (int i = start; i <= end; i++) {
			lineNum = i - 20 * (page - 1);
			
			if (lineNum < 10) {
				cout << " " << lineNum << "| " << lines[i].buf << endl;
			}
			else {
				cout << lineNum << "| " << lines[i].buf << endl;

			}
		}
	}

	string get_command() {
		string command;
		cin >> command;

		return command;
	}

	void show_interface() {
		string command;
			show_page(pageNumber);

			cout << "------------------------------------------------------------------------------------------" << endl;
			cout << "n:next page , p:previous page, i:insert, d:delete, c:change, s:search, t:save and quit " << endl;
			cout << "------------------------------------------------------------------------------------------" << endl;
			cout << "(consol message) " << consolMessage << endl;
			cout << "------------------------------------------------------------------------------------------" << endl;
			cout << "input:";
	}

	void setAction(Action* action_) {
		action = action_;
	}

	void doAction() {
		action->doAction();
	}

	void text_to_vector() {
		ifstream file;
		file.open(filename);
		string word, prev;
		int currentByte = 1;
		char buf[MAXBYTE] = "";
		Buffer buffer;

		lines.push_back(buffer);	// 줄 맞춤 편리성 위함

		word.clear();
		prev.clear();

		while (file >> word) {

			if (!prev.empty()) {
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
				word.clear();
				continue;
			}

			int  j = 0;

			for (int i = currentByte - word.length() - 1; i < currentByte - 1; i++) {
				buffer.buf[i] = word.at(j);
				j++;
			}
			word.clear();
		}
		lines.push_back(buffer);
		file.close();
	}

	void clear_buffer(Buffer& buffer) {
		for (int i = 0; i <= 151; i++) {
			buffer.buf[i] = '\0';
		}
	}
};


void string_data_handling(string command, vector<string>& splitedData) {
	string data = command.substr(2, command.length() - 3);
	string stringBuf;
	istringstream ss(data);

	while (getline(ss, stringBuf, ',')) {
		splitedData.push_back(stringBuf);
	}
}

void main() {
	string command;
	string data;	// 사용자 입력값 데이터 추출용
	string stringBuf;
	vector<string> splitedData;
	Term *t = new Term("test.txt");
	Action* next = new NextPage;
	Action* prev = new PrevPage;
	Insert* insert = new Insert;
	Delete* del = new Delete;
	Search* search = new Search;
	Change* change = new Change;
	Action* saveQuit = new SaveQuit;
	int a, b;

	do {
		splitedData.clear();
		t->show_interface();
		command = t->get_command();
		string_data_handling(command, splitedData);

		consolMessage.clear();

		if (!command.compare("n")) {
			t->setAction(next);
			t->doAction();
		}
		else if (!command.compare("p")) {
			t->setAction(prev);
			t->doAction();
		}
		else if (command.at(0) == 'i') {
			// insert 구현
			stringstream(splitedData[0]) >> a;
			stringstream(splitedData[1]) >> b;
			insert->i(a, b, splitedData[2]);
			t-> setAction(insert);
			t-> doAction();
		}
		else if (command.at(0) == 'd') {
			// delete 구현
			stringstream(splitedData[0]) >> a;
			stringstream(splitedData[1]) >> b;
			del->d(a, b);
			t-> setAction(del);
			t-> doAction();
		}
		else if (command.at(0) == 's') {
			// 텍스트 파일 처음부터 탐색, 첫번째 출력 창의 첫번째 라인에 위치하도록 출력
			search->s(splitedData[0]);
			t-> setAction(search);
			t-> doAction();
		}
		else if (command.at(0) == 'c') {
			// 변경하고 싶은 단어 모두 다른 단어로 변경
			change->c(splitedData[0], splitedData[1]);
			t->setAction(change);
			t->doAction();
		}
		else if (!command.compare("t")){
			t->setAction(saveQuit);
			t->doAction();
		}
		else {
			consolMessage = "error: invalid command";
		}
		cout << "------------------------------------------------------------------------------------------" << endl;
		Sleep(100);
	//	system("cls");

	} while (command.compare("t"));
}