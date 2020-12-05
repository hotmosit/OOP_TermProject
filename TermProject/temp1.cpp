#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include <sstream>

using namespace std;

struct Buffer {
	char buf[76] = "";
};

int main() {

	std::ifstream file;
	file.open("test.txt");
	string word, prev, sentance;
	int line = 1;
	int const MAXBYTE = 75;
	int current_byte = 1;
	char buf[75] = "";
	Buffer buffer;

	vector<Buffer> lines;
	lines.push_back(buffer);

	word.clear();
	sentance.clear();
	prev.clear();

	while (file >> word /*&& line <= 20*/)
	{	
		if (word.length() > 2 && word.at(word.length()-1) == '\'') {
			cout << "activated" << '\n';
			word.replace(word.end() - 1, word.end(), "\'s");
		}

		sentance = sentance + word;

		if (word.at(word.length() - 1) == '.' && word.length() > 2) {

//			cout << sentance << '\n';

			istringstream ss(sentance);

			while (getline(ss, word, ' ')) {
				if (!prev.empty()) {
					prev += ' ';
					for (int i = 0; i < prev.length(); i++) {
						buffer.buf[i] = prev.at(i);
					}
					current_byte += prev.length();

					//	cout << "there was prev: " << prev << '\n' << "current buf : "<< buf << endl ;
					prev.clear();
				}
				word += ' ';
				current_byte += word.length();

				//	cout << word << "-> byte : " << word.length() << ", current byte :" << current_byte << endl;

				if (current_byte / MAXBYTE >= 1) {
					// 75byte를 넘길 시 
					prev = word;
					current_byte = 1;
		/*			if (line < 10) {
						cout << " " << line << "| " << buffer.buf << '\n';
					}
					else {
						cout << line << "| " << buffer.buf << '\n';
					}
*/
					lines.push_back(buffer);

					for (int i = 0; i < 75; i++) {
						buffer.buf[i] = '\0';
					}
					line++;
					word.clear();
					continue;
				}
				int  j = 0;

				for (int i = current_byte - word.length() - 1; i < current_byte - 1; i++) {
					buffer.buf[i] = word.at(j);
					j++;
				}

				word.clear();
				sentance.clear();
			}
// 추가
//			lines.push_back(buffer);
/*			for (int i = 0; i < 75; i++) {
				buffer.buf[i] = '\0';
			}
*/		}
		else {
			sentance += ' ';

		}

		word.clear();
	}

	lines.push_back(buffer);

	for (int i = 1; i < lines.size(); i++) {
		cout << i  << "| " << lines[i].buf << '\n';
	}

	file.close();


	return 0;
}