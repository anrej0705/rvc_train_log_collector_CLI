// rvc_logs_extraction.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <list>
#include <conio.h>
#include <string_view>
#include <sstream>
#include "OpenXLSX.hpp"

struct rvc_tags
{
	std::string tag_disc = "loss_disc=";
	std::string tag_gen = "loss_gen=";
	std::string tag_fm = "loss_fm=";
	std::string tag_mel = "loss_mel=";
	std::string tag_kl = "loss_kl=";
}tags;
class rvc_logs
{
	private:
		std::string filestream;
		std::list<float> loss_disc;
		std::list<float> loss_gen;
		std::list<float> loss_fm;
		std::list<float> loss_mel;
		std::list<float> loss_kl;
		std::stringstream buffer;
		std::ifstream inFile;
		int steps;
		OpenXLSX::XLDocument doc;
		auto read_file(std::string_view path)->std::string;
	public:
		rvc_logs()
		{
			steps = 0;
			filestream = read_file("train.log");
		}
		rvc_logs(std::string file_path)
		{
			steps = 0;
			filestream = read_file(file_path);
		}
		~rvc_logs()
		{
			inFile.close();
			doc.save();
			doc.close();
		}
		void parseVals();
		void saveExcel();
};

auto rvc_logs::read_file(std::string_view path)->std::string
{
	constexpr auto read_size = std::size_t(4096);
	auto stream = std::ifstream(path.data());
	stream.exceptions(std::ios_base::badbit);
	if (not stream)
		throw std::ios_base::failure("file does not exists");
	auto out = std::string();
	auto buf = std::string(read_size, '\0');
	while (stream.read(&buf[0], read_size))
		out.append(buf, 0, static_cast<unsigned int>(stream.gcount()));
	out.append(buf, 0, static_cast<unsigned int>(stream.gcount()));
	return out;
}

void rvc_logs::parseVals()
{
	//Поиск последовательно loss_disc => loss_gen => loss_fm => loss_mel => loss_kl
	int indexIn = 0;
	int readIn = 0;
	std::string readVal = "";
	float fVal = 0.0f;
	for (;;++steps)
	{
		//Ищем loss_disc и дальше читаем значение которое преобразуем в float и пихаем в контейнер
		indexIn = filestream.find(tags.tag_disc, indexIn);
		//Так как loss_disc всегда идёт первым то если новой строки нет, то поиск вернет 0
		//И значит дальше искать нет смысла
		if (indexIn == -1)
		{
			//indexIn = steps;
			break;
		}
		readIn = indexIn + tags.tag_disc.size();
		readVal = filestream.substr(readIn, filestream.find(",", indexIn) - readIn);
		fVal = std::stof(readVal);
		loss_disc.push_back(fVal);

		//Читаем loss_gen
		indexIn = filestream.find(tags.tag_gen, indexIn);
		readIn = indexIn + tags.tag_gen.size();
		readVal = filestream.substr(readIn, filestream.find(",", indexIn) - readIn);
		fVal = std::stof(readVal);
		loss_gen.push_back(fVal);

		//Читаем loss_fm
		indexIn = filestream.find(tags.tag_fm, indexIn);
		readIn = indexIn + tags.tag_fm.size();
		readVal = filestream.substr(readIn, filestream.find(",", indexIn) - readIn);
		fVal = std::stof(readVal);
		loss_fm.push_back(fVal);

		//Читаем loss_mel
		indexIn = filestream.find(tags.tag_mel, indexIn);
		readIn = indexIn + tags.tag_mel.size();
		readVal = filestream.substr(readIn, filestream.find(",", indexIn) - readIn);
		fVal = std::stof(readVal);
		loss_mel.push_back(fVal);

		//Читаем loss_kl
		indexIn = filestream.find(tags.tag_kl, indexIn);
		readIn = indexIn + tags.tag_kl.size();
		readVal = filestream.substr(readIn, filestream.find(",", indexIn) - readIn);
		fVal = std::stof(readVal);
		loss_kl.push_back(fVal);
	}
}
void rvc_logs::saveExcel()
{
	doc.create("train_stats.xlsx");
	auto wks = doc.workbook().worksheet("Sheet1");
	//wks.cell("A1").value() = "epoch";
	wks.cell("A1").value() = "loss_disc";
	wks.cell("B1").value() = "loss_gen";
	wks.cell("C1").value() = "loss_fm";
	wks.cell("D1").value() = "loss_mel";
	wks.cell("E1").value() = "loss_kl";
	for (uint16_t rows = static_cast<uint16_t>(loss_disc.size()); rows != 0; --rows)
	{
		//wks.cell("A" + std::to_string(rows + 1)).value() = rows;
		std::cout << "step=" << rows << " loss_disc=" << loss_disc.back() << " ";
		wks.cell("A" + std::to_string(rows + 1)).value() = loss_disc.back();
		loss_disc.pop_back();
		std::cout << "loss_gen=" << loss_gen.back() << " ";
		wks.cell("B" + std::to_string(rows + 1)).value() = loss_gen.back();
		loss_gen.pop_back();
		std::cout << "loss_fm=" << loss_fm.back() << " ";
		wks.cell("C" + std::to_string(rows + 1)).value() = loss_fm.back();
		loss_fm.pop_back();
		std::cout << "loss_mel=" << loss_mel.back() << " ";
		wks.cell("D" + std::to_string(rows + 1)).value() = loss_mel.back();
		loss_mel.pop_back();
		std::cout << "loss_kl=" << loss_kl.back() << " ";
		wks.cell("E" + std::to_string(rows + 1)).value() = loss_kl.back();
		loss_kl.pop_back();
		std::cout << std::endl;
	}
	std::cout << "Job done! Press eny key to exit" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		std::cout << "Just drop train.log file into me" << std::endl;
	std::cout << "Open file for job: " << argv[1] << std::endl;
	rvc_logs log(argv[1]);
	log.parseVals();
	log.saveExcel();
	//_getch();
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
