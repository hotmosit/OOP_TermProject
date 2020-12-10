#define _CRT_SECURE_NO_WARNINGS

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
int searchedLine = -1;
string consolMessage = "";


class Action {
public:
	virtual void doAction() = 0;

	void clear_buffer(Buffer& buffer) {
		for (int i = 0; i <= 151; i++) {
			buffer.buf[i] = '\0';
		}
	}

	void reWrite() {
		vector<Buffer> temp;
		string word, prev, sentance;
		int currentByte = 1;
		char buf[75] = "";
		Buffer buffer;

		word.clear();
		prev.clear();
		sentance.clear();

		temp.push_back(buffer);

		for (int i = 0; i < lines.size(); i++) {
			sentance = lines[i].buf;
			istringstream ss(sentance);

			while (getline(ss, word, ' ')) {
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
					temp.push_back(buffer);
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
			word.clear();
			sentance.clear();
		}

		temp.push_back(buffer);

		lines.clear();
		lines.assign(temp.begin(), temp.end());
	}
};

class NextPage :public Action {
public:
	void doAction() override {
		// 허용 페이지가 넘을때 
		if (lines.size() / 20 < pageNumber) {
			consolMessage = "This is the last page!";
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
			consolMessage = "This is the first page!";
			return;
		}
		--pageNumber;
	}
};

class Insert : public Action {
public:
	Insert(int a, int b, string& s) : line((pageNumber - 1) * 20 + a), col(b), word(s) {
		if (a > 20) {
			consolMessage = "error: line out of range ( invalid line : 1<= line <= 20)";
			throw 0;
		}
	}
	~Insert() {};

	void doAction() override {
		string temp = lines[line].buf;
		int wordCount = 0;
		int i;

		for (i = 0; i < temp.length(); i++) {
			// 공백의 숫자를 통해 단어 개수 파악

			if (temp.at(i) == ' ') {
				++wordCount;
			}

			if (wordCount + 1 == col) {		
				// 원하는 위치 찾는 경우
				break;
			}
		}


		if (wordCount + 1 < col || col < 1) {
			consolMessage = "error: placement number input out of range";
			throw 0;
		}

		temp.insert(i + 1, word + " ");
		strcpy(lines[line].buf, temp.c_str());

		// 문장 삽입 후 75 바이트 크기를 넘을 수 있다.
		reWrite();

	}
public:
	int line, col;
	string word;
};

class Delete : public Action {
public:

	Delete(int a, int b) : line((pageNumber - 1) * 20 + a), col(b) {
		if (a > 20) {
			// 줄 범위 추가 
			consolMessage = "error: line out of range ( invalid line : 1<= line <= 20)";
			throw 0;
		}
	}
	~Delete() {}

	void doAction() override {
		string deletedWord;
		string temp = lines[line].buf;
		int wordCount = 0;
		int k;
		for (k = 0; k < temp.length(); k++) {
			// 공백으로 단어 위치 찾기 
			if (temp.at(k) == ' ') {
				++wordCount;
			}
			if (wordCount + 1 == col) {
				break;
			}
		}


		if (wordCount + 1 < col || col < 1) {
			consolMessage = "error: delete target number input out of range";
			throw 0;
		}

		for (int i = k + 1; lines[line].buf[i] != ' '; i++) {
			// 단어 끝나기 전까지 삭제할 단어 정보 수집
			deletedWord += lines[line].buf[i];
		}

		temp.erase(k + 1, deletedWord.length() + 1);
		strcpy(lines[line].buf, temp.c_str());

		reWrite();
	}

public:
	int line, col;

};

class Change : public Action {
public:
	Change(string oldWord_, string newWord_) : oldWord(oldWord_), newWord(newWord_) {}
	~Change() {}

	void doAction() {
		int lineStart = (pageNumber - 1) * 20 + 1;
		int lineEnd = lineStart + 19;
		string temp;

		for (int i = lineStart; i <= lineEnd; i++) {
			temp = lines[i].buf;
			temp = replaceAll(temp, oldWord, newWord);
			strcpy(lines[i].buf, temp.c_str());
		}

		reWrite();
	}
private:
	string replaceAll(string& str, const string& from, const string& to) {
		size_t start_pos = 0; //string처음부터 검사
		while ((start_pos = str.find(from, start_pos)) != string::npos)  //from을 찾을 수 없을 때까지
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
		}
		return str;
	}

public:
	string oldWord, newWord;
};

class Search : public Action {
public:
	Search(string s_) {
		target = s_;
	}
	~Search() {}
	void doAction() {
		string s;

		for (int i = 0; i < lines.size(); i++) {
			s = lines[i].buf;

			if (s.find(target) <= 75) {
				searchedLine = i;
				break;
			}
		}

	}
public:
	string target;
};

class SaveQuit : public Action {
public:
	void doAction() {
		ofstream write("test.txt");

		for (int i = 0; i < lines.size(); i++) {
			string temp = lines[i].buf;
			write << temp;
		}
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
		int start;
		if (searchedLine != -1) {
			start = searchedLine;
		}
		else {
			start = 20 * (page - 1) + 1;
		}

		int end = start + 19;

		if (end > lines.size()) {
			end = lines.size() - 1;
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

		searchedLine = -1;
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
				// 이전 줄에서 75 바이트를 넘어선 단어에 대한 처리 
				for (int i = 0; i < prev.length(); i++) {
					buffer.buf[i] = prev.at(i);
				}
				currentByte += prev.length();
				prev.clear();
			}

			word += ' ';
			currentByte += word.length();

			if (currentByte / MAXBYTE >= 1) {
				prev = word;				// 75 BYTE를 넘기는 경우 다음 줄에 넘기기 위해 저장
				currentByte = 1;
				lines.push_back(buffer);
				clear_buffer(buffer);
				word.clear();
				continue;
			}

			int  j = 0;

			for (int i = currentByte - word.length() - 1; i < currentByte - 1; i++) {
				// 단어를 현재 줄에 저장
				buffer.buf[i] = word.at(j);
				j++;
			}
			word.clear();
		}
		lines.push_back(buffer);	// 마지막 줄에 대한 처리 
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
		if (stringBuf.length() > MAXBYTE) {
			consolMessage = "string parameter must be under 75 byte";
			throw 0;
		}
	
		for (int i = 0; i < stringBuf.length(); i++) {
			if (stringBuf[i] == ' ') {
				throw 0;
			}
		}

		splitedData.push_back(stringBuf);

	}


}

void main() {
	string command;
	string data;	// 사용자 입력값 데이터 추출용
	string stringBuf;
	vector<string> splitedData;
	Term* t = new Term("test.txt");
	Action* next = new NextPage;
	Action* prev = new PrevPage;;
	Action* saveQuit = new SaveQuit;
	int a, b;

	do {
		splitedData.clear();
		t->show_interface();
		command = t->get_command();

		consolMessage.clear();
		try {
			if (command.at(0) == 'n') {
				t->setAction(next);
				t->doAction();
			}
			else if (command.at(0) == 'p') {
				t->setAction(prev);
				t->doAction();
			}
			else if (command.at(0) == 'i') {
				// insert 구현

				string_data_handling(command, splitedData);
				stringstream(splitedData[0]) >> a;
				stringstream(splitedData[1]) >> b;

				Insert* insert = new Insert(a, b, splitedData[2]);
				
				t->setAction(insert);
				t->doAction();

				delete(insert);

			}
			else if (command.at(0) == 'd') {
				// delete 구현			
				string_data_handling(command, splitedData);
				stringstream(splitedData[0]) >> a;
				stringstream(splitedData[1]) >> b;

				Delete* del = new Delete(a, b);
			
				t->setAction(del);
				t->doAction();

				delete del;
			}
			else if (command.at(0) == 's') {
				// 텍스트 파일 처음부터 탐색, 첫번째 출력 창의 첫번째 라인에 위치하도록 출력
				string_data_handling(command, splitedData);
				Search* search = new Search(splitedData[0]);
				t->setAction(search);
				t->doAction();

				delete search;
			}
			else if (command.at(0) == 'c') {
				// 변경하고 싶은 단어 모두 다른 단어로 변경
				string_data_handling(command, splitedData);

				Change* change = new Change(splitedData[0], splitedData[1]);
				t->setAction(change);
				t->doAction();
				delete(change);
			}
			else if (!command.compare("t")) {

				t->setAction(saveQuit);
				t->doAction();
			}
			else {
				consolMessage = "error: invalid command";
			}
		}
		catch (...) {
			if (consolMessage.empty()) {
				consolMessage = "error: invalid input, check blank, invalid input type, etc...";
			}
		}
		cout << "------------------------------------------------------------------------------------------" << endl;
		Sleep(100);

		system("cls");
	} while (command.compare("t"));

	delete next;
	delete prev;
	delete saveQuit;
}