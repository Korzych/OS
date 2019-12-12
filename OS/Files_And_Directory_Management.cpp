//Krzysio
#include "Files_And_Directory_Management.hpp"
#include "CPU_Scheduling.hpp"

#include "Headers.h"
int Filesystem::findFreeBlock(std::bitset<Drive::blocks> &bt)
{
	for (int i = 0; i < bt.size(); i++)
	{
		if (bt[i] == false) return i;
	}
	return -1;
}
bool Filesystem::fileExists(const string &filename)
{
	if (maincatalogue.size() != 0) 
	{
		for (int i = 0; i < maincatalogue.size(); i++)
		{
			if (maincatalogue.at(i).name == filename)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}
bool Filesystem::createFile(const string &filename)
{

	if (fileExists(filename))
	{
		std::cout << "E001:Plik o podanej nazwie juz istnieje";
		return false;
	}
	else
	{
		//Tworzenie pliku
		//Je�li mamy 0 lub 1 wolny blok to odrzuca operacj� gdy� przy tworzeniu pliku potrzebujemy przynajmniej 2 wolne bloki 
		if (countFreeBlocks(blockTable) < 2)
		{
			std::cout << "E002:Za malo miejsca na dysku";
			return false;
		}
		else
		{
			//Blok indeksowy
			int indexblock = hireFreeBlock(blockTable);
			//1 automatycznie inicjowany blok danych
			int data = hireFreeBlock(blockTable);
			//Umieszczanie wpisu pliku w katalogu
			File tempfile(filename, indexblock);
			tempfile.towrite = data * 32;
			maindrive.drivespace[indexblock * 32] = data;
			data = maindrive.drivespace[indexblock * 32];
			maincatalogue.push_back(tempfile);
		}
		return true;

	}
}
//ZLICZANIE WOLNYCH BLOK�W//DZIA�A
int Filesystem::countFreeBlocks(std::bitset<Drive::blocks> &bt)
{
	int fb = 0;
	for (int i = 0; i < bt.size(); i++)
	{
		if (bt[i] == false) fb++;
	}
	return fb;
}
//"ZATRUDNIANIE WOLNEGO BLOKU"//DZIA�A
int Filesystem::hireFreeBlock(std::bitset<Drive::blocks> &bt) //"ZATRUDNIANIE" WOLNYCH BLOK�W// DZIA�A
{
	int bn = findFreeBlock(bt);
	if (bn != -1)
	{
		bt[bn] = true;
		return bn;
	}
	return -1;

	//std::cout << "Rezerwuje:" << bn << std::endl;

}
//WY�WIETLA STATYSTYKI//DZIA�A
void Filesystem::stats()
{
	std::cout << "\nWolne bloki dyskowe:" << countFreeBlocks(blockTable) << std::endl;
	std::cout << "Calkowite bloki dyskowe: " << Drive::blocks << std::endl;
	std::cout << "Rozmiar dysku:" << Drive::drivesize << std::endl;
}
//DODAJE PLIK DO TABLICY OTWARTYCH PLIK�W
bool Filesystem::openFile(const string &filename)
{
	return openFile(filename, System::CPU.getRunningPID());
}
bool Filesystem::openFile(const string &filename, const string &pid)
{
	//SPRAWDZAM CZY PLIK WGL ISTNIEJE
	if (fileExists(filename))
	{
		for (int i = 0; i < maincatalogue.size(); i++)
		{
			if (maincatalogue[i].name == filename)
			{
				maincatalogue[i].sem.wait_sem(pid);

				openfiletable.insert(std::pair <string, string>(filename, pid));
				break;
			}
		}

	}


	//WAIT NA SEMAFORZE;
	/*

	//JE�LI PLIK JU� ZNAJDUJE SI� W TABLICY OTWARTYCH PLIK�W:
	if (openfiletable.count(filename) >= 1)
	{
		//JE�ELI PLIK JEST OTWARTY PRZEZ TEN PROCES
		if (openfiletable[filename] == pid)
		{
			std::cout << "Plik jest juz otwarty przez ten proces" << std::endl;
		//	return false;
		}
		//JE�ELI PLIK JEST OTWARTY PRZEZ INNY PROCES
		if (openfiletable[filename] != pid)
		{
			std::cout << "Plik jest juz otwarty przez inny proces" << std::endl;
			//return false;
		}
	}
	else//JE�LI PLIKU NIE MA TO TWORZYMY PLIK
	{
		std::cout << "PLIK OTWARTY" << std::endl;

		openfiletable.insert(std::pair<string, string> (filename, pid));
	//	return true;
	}*/

	else
	{
		std::cout << "Plik nie istnieje" << std::endl;
	}

	return false;
}
//USUWA PLIK Z TABLICY OTWARTYCH PLIK�W
bool Filesystem::closeFile(const string &filename)
{
	return closeFile(filename, System::CPU.getRunningPID());
}
bool Filesystem::closeFile(const string &filename, const string &pid)
{
	//SPRAWDZANIE CZY PLIK ISTNIEJE
	if (fileExists(filename))
	{

		for (int i = 0; i < maincatalogue.size(); i++)
		{
			if (maincatalogue[i].name == filename)
			{
				openfiletable.erase(filename);
				maincatalogue[i].sem.signal_sem();


			}
		}

	}/*
		//CZY PLIK ZNAJDUJE SI� W TABLICY OTWARTYCH PLIK�W
		if (openfiletable.count(filename) >= 1)
		{
			//JE�ELI PLIK JEST OTWARTY PRZEZ TEN PROCES
			if (openfiletable[filename] == pid)
			{
				//TO MOG� GO ZAMKN��
				openfiletable.erase(filename);
				std::cout << "Zamykam plik" << std::endl;
				return true;
			}
			//JE�ELI PLIK JEST OTWARTY PRZEZ INNY PROCES
			if (openfiletable[filename] != pid)
			{
				//TO NIE MO  GO ZAMKN��
				std::cout << "Brak dostepu do pliku" << std::endl;
				return false;
			}*/


			//Sprawdzanie czy plik jest w tablicy otwartych plik�w
			//JE�LI PLIK  ZNAJDUJE SI� W TABLICY OTWARTYCH PLIK�W:

			//JE�ELI PLIK JEST OTWARTY PRZEZ INNY PROCES TO NIE MO�ESZ GO ZAMKN��
			//JE�ELI PLIK JEST OTWARTY PRZEZ TEN PROCES TO MO�ESZ GO ZAMKN��
		//JE�LI PLIKU NIE MA TO NIE MA CZEGO ZAMYKA�


	else
	{
		std::cout << "Plik nie istnieje" << std::endl;
		return false;
	}




	return false;
}
bool Filesystem::renameFile(const string &filename, const string &newfilename)
{
	return renameFile(filename, newfilename, System::CPU.getRunningPID());
}
bool Filesystem::renameFile(const string &filename, const string &newfilename, const string &pid)
{
	if (verify(filename))
	{

		if (fileExists(newfilename))
		{
			std::cout << "Plik o nazwie docelowej juz istnieje";
			return false;
		}
		else
		{
			for (int i = 0; i < maincatalogue.size(); i++)
			{
				if (maincatalogue[i].name == filename)
				{
					int adres = maincatalogue[i].adres;
					int towrite = maincatalogue[i].towrite;
					maincatalogue.erase(maincatalogue.begin() + (i));
					File tempfile(newfilename, adres);
					tempfile.towrite = towrite;
					maincatalogue.push_back(tempfile);
				}
			}

			return true;

		}
	}
	return false;

}
bool Filesystem::overwriteFile(const string &filename, const string &content)
{
	return overwriteFile(filename, content, System::CPU.getRunningPID());
}
bool Filesystem::overwriteFile(const string &filename, const string &content, const string &pid)
{
	if (verify(filename))
	{

		int indexblock;
		File tempfile;
		int at;
		for (int i = 0; i < maincatalogue.size(); i++)
		{
			if (maincatalogue[i].name == filename)
			{
				tempfile = maincatalogue[i];
				indexblock = tempfile.adres * 32;//WSKAZUJE NA 1 POLE INDEKSOWE 
				at = i;
				maincatalogue[i].toread = 0;
				maincatalogue[i].towrite = 0;
			}
		}
		int b = 0;
		for (int i = 0; i < 32; i++)
		{
			if (maindrive.drivespace[i + indexblock] == 0)
			{
				break;
			}
			else
				b++;
		}
		//	std::cout << "B:" << b << std::endl;
		//	std::cout << content.size()/32+1 << "-To wymagane bloki" << (countFreeBlocks(blockTable) + b);
		if (((content.length() / 32) + 1) > (countFreeBlocks(blockTable) + b))
		{

			std::cout << "E007:Za malo miejsca na dysku" << std::endl;
			return false;

		}

		//USUWANIE DANYCH
		for (int i = 0; i < 32; i++)
		{
			if (maindrive.drivespace[i + indexblock] == 0)
			{
				break;
			}
			else
			{
				int blocknumber = maindrive.drivespace[i + indexblock];//NUMER OBECNIE ZWALNIANEGO BLOKU DYSKOWEGO 
				int currentblock = 32 * blocknumber;//ADRES TEGO BLOKU
				for (int j = 31; j > -1; j--)
				{
					maindrive.drivespace[currentblock + j] = 0;//ZERUJE POLA DANYCH
				}
				blockTable[blocknumber] = false;//ZWALNIA BLOK DANYCH
				maindrive.drivespace[i + indexblock] = 0; //USUWA BLOK Z BLOKU INDEKSOWEGO
				//USUWA W WEKTORZE
			}


		}


		int g;

		std::vector<int> blockvector;
		int rozmiar = content.size();
		for (int i = 0; i < ((rozmiar + 1) / 32); i++)//Rezerwacja blok�w danych i przypisanie do bloku indeksowego 
		{
			int f = hireFreeBlock(blockTable);
			if (f != -1)
			{
				blockvector.push_back(f * 32);
				g = f * 32;//ZAPISUJE ADRESY BLOK�W DANYCH DO ZAPISU
				maindrive.drivespace[indexblock + i] = f;
			}
			else
			{
				std::cout << "E001:ZA MALO MIEJSCA NA DYSKU";
				return false;
			}


		}

		std::vector <char> towrite;
		std::copy(content.begin(), content.end(), std::back_inserter(towrite));
		int j = 0;
		int ij = 0;
		int a = 0;
		for (int i = 0; i < content.size(); i++)
		{
			if (j >= blockvector.size())
			{
				break;
			}
			if (i >= 32)
			{
				i = 0;
				j++;
				if (j >= blockvector.size())
				{
					break;
				}
			}

			maindrive.drivespace[blockvector[j] + i] = towrite[ij];

			ij++;
			if (ij == towrite.size())
			{
				break;
			}
			a = i + 1;
		}
		//std::cout << "JESTEM:" << j << " A rozmiar to:" << blockvector.size();

		ij = blockvector[j - 1]; //TU NIE DZIA�A

		maincatalogue[at].towrite = ij + a + 1;
		//ZAPIS DANYCH DO BLOK�W




	}
	return true;
}
bool Filesystem::writeToFile(const string &filename, const string &content)
{
	return writeToFile(filename, content, System::CPU.getRunningPID());
}
bool Filesystem::writeToFile(const string &filename, const string &content, const string &pid)
{
	if (verify(filename))
	{


		int rozmiar = content.length();
		//std::cout << "\nRozmiar Pliku:" << rozmiar << std::endl;
		if (((rozmiar / 32)) > countFreeBlocks(blockTable))
		{
			std::cout << "E003:Za malo miejsca na dysku" << std::endl;
			return false;

		}

		//std::cout << "zapisuje do pliku";
		//ADRES BLOKU INDEKSOWEGO 
		int blokindeksowy;
		File tempfile;
		int at;
		for (int i = 0; i < maincatalogue.size(); i++)
		{
			if (maincatalogue[i].name == filename)
			{
				tempfile = maincatalogue[i];
				blokindeksowy = tempfile.adres * 32;
				at = i;
			}
		}
		int blokpamieci = (maindrive.drivespace[blokindeksowy]);//PIERWSZY ADRES Z BLOKU INDEKSOWEGO
		int adrespamieci = 32 * blokpamieci;

		int placetowrite = tempfile.towrite;

		int requiredblocks;
		if (rozmiar != 0)
		{
			requiredblocks = ceil((((double)rozmiar + 1) - (32 - placetowrite % 32)) / 32); //nieefektywne ale dzia�a	
		}

		if (rozmiar == 0)
		{
			requiredblocks = 0;
		}

		for (int i = 0; i < requiredblocks; i++)//Rezerwacja blok�w danych i przypisanie do bloku indeksowego 
		{
			int f = hireFreeBlock(blockTable);
			if (f == -1)
			{
				std::cout << "\nE004:Brak wolnego miejsca na dysku\n";
				return false;
			}
			else
			{
				maindrive.drivespace[blokindeksowy + i + 1] = f;
			}

		}
		std::vector <char> towrite;
		int  it;
		it = 0;
		std::vector<int> blokidanych;
		while (maindrive.drivespace[blokindeksowy + it] != 0)
		{
			int f = maindrive.drivespace[blokindeksowy + it];
			blokidanych.push_back(f);
			it++;
		}
		std::copy(content.begin(), content.end(), std::back_inserter(towrite));
		int jmp = 0;
		int value;
		//Zapis do pliku
		for (int a = 0; a < towrite.size(); a++)
		{
			maindrive.drivespace[placetowrite] = towrite[a];
			placetowrite++;
			if (placetowrite % 32 == 0 && placetowrite != blokindeksowy)
			{

				if (jmp < blokidanych.size())
				{

					while (placetowrite > (blokidanych[jmp] * 32) + 31)//je�eli zapisano wi�cej ni� 1 blok TO NIE B�DZIE BRA� 1 BLOKU DO ZAPISU TYLKO DALSZE 
					{
						jmp++;
						if (jmp < blokidanych.size())
						{

							break;
						}

					}
					placetowrite = 32 * blokidanych[jmp];
					//	std::cout << "ZAPIS NA:" << placetowrite;
				}
				//Przesuwamy si� do nast�pnego bloku dyskowego
				//placetowrite ==  maindrive.drivespace[lastusedindex-1];
			}
			//	std::cout << placetowrite-1<<" ";

		}
		maincatalogue[at].towrite = placetowrite;
		return true;
	}
	else
	{
		std::cout << "E005:Blad zapisu";
		return false;
	}
}
bool Filesystem::readFile(const string &filename, int length, string &content)
{
	return readFile(filename, System::CPU.getRunningPID(), length, content);
}
bool Filesystem::readFile(const string &filename, const string &pid, int length, string &content)//LEPIEJ CHYBA PRZEZ REFERENCJE 
{
	if (verify(filename))
	{
		int blokindeksowy;
		File tempfile;
		int at;
		int read;
		for (int i = 0; i < maincatalogue.size(); i++)//P�TLA SZUKAJ�CA PLIKU W KATALOGU
		{
			if (maincatalogue[i].name == filename)
			{
				tempfile = maincatalogue[i];
				blokindeksowy = tempfile.adres * 32;//Adres bloku indeksowego
				at = i;//MIEJSCE W KATALOGU
				read = maincatalogue[i].toread;
			}
		}
		std::vector <int> toreadvector;
		string tempstr = "";
		//ODCZYT PLIKU
		for (int i = 0; i < 32; i++)//P�TLA KT�RA WYPE�NIA WEKTOR BLOK�W Danych
		{
			if (maindrive.drivespace[blokindeksowy + i] == 0)
			{
				break;
			}
			else
			{
				int f = maindrive.drivespace[blokindeksowy + i] * 32;
				toreadvector.push_back(f);
			}
		}

		int blok;
		int sup;//Zmienna pomocnicza
		int c = 0;//Liczba odczytanych znak�w
		int i = 0;//Czytam w i-tym bloku pliku
		while (c != length)
		{
			blok = toreadvector[i];
			if (read < ((i + 1) * 32) && (read >= (i * 32))) {//JE�ELI ZMIENNA ODCZYTU ZNAJDUJE SI� W OBECNYM BLOKU TO //�LE CO� XD
				sup = read % 32;
				if (maindrive.drivespace[blok + sup] != 0)
				{

					tempstr = tempstr + maindrive.drivespace[blok + sup];
					read++;
					c++;
				}
				else
				{
					break;
				}
			}
			else //Je�eli zmienna odczytu nie znajduje si� w tym bloku to zmieniamy blok
			{
				i++;
				if (i >= toreadvector.size())
				{
					break;
				}
			}


		}
		content = tempstr;
		maincatalogue[at].toread = read;
		return true;
	}
	else
	{
		return false;
	}
}
bool Filesystem::deleteFile(const string &filename)
{
	return deleteFile(filename, System::CPU.getRunningPID());
}
bool Filesystem::deleteFile(const string &filename, const string &pid)
{
	if (verify(filename))
	{
		File tempfile;
		int at;
		int indexblock;
		for (int i = 0; i < maincatalogue.size(); i++)//WYSZUKUJE PLIK I POBIERAM PODSTAWOWE DANE
		{
			if (maincatalogue[i].name == filename)
			{
				tempfile = maincatalogue[i];
				indexblock = tempfile.adres * 32;
				at = i;
			}
		}
		std::vector <int> removalvector;
		for (int i = 0; i < 32; i++)
		{
			if ((int)maindrive.drivespace[indexblock + i] == 0)
			{
				break;
			}
			else
			{
				removalvector.push_back((int)maindrive.drivespace[indexblock + i]);
				maindrive.drivespace[indexblock + i] = 0;
			}
		}

		//USUWANIE PLIKU Z DYSKU 
		int a = -1;
		int b = 0;
		for (int i = 0; i < removalvector.size(); i++)
		{


			int blocknumber = removalvector[i]; //Numer obecnie usuwanego bloku dyskowego
			int currentblock = 32 * blocknumber;//Adres tego bloku
			for (int j = 31; j > -1; j--)
			{
				maindrive.drivespace[currentblock + j] = 0;//ZERUJE WSZYSTKIE POLA DANYCH BLOKU
			}
			blockTable[blocknumber] = false;//PO WYZEROWANIU DANYCH BLOK JEST ZWALNIANY
			maindrive.drivespace[i + indexblock] = 0; //W BLOKU INDEKSOWYM USUWAMY BLOK DYSKOWY
			//USUWA W WEKTORZE



		}
		blockTable[tempfile.adres] = false;//ZWOLNIENIE BLOKU DYSKOWEGO
		maincatalogue[at].sem.delete_sem();//ZWALNIAM KOLEJK�
		maincatalogue.erase(maincatalogue.begin() + at);

	}

	return true;
}

bool Filesystem::verify(const string &filename)
{
	if (!fileExists(filename))
	{
		std::cout << "E006:Plik o podanej nazwie nie istnieje";
		return false;
	}
	return true;
}

void Filesystem::displaydrivecontent()
{
	std::cout << std::endl;
	for (int i = 0; i < 32; i++)
	{
		displayblock(i);
	}
}
void Filesystem::displaycatalogue()
{
	std::cout << "\nNazwa Pliku\t\tAdres\t\tMiejsce Odczytu\t\tMiejsce Zapisu\t\tWartosc semafora";
	for (auto e : maincatalogue)
	{
		std::cout << std::endl << e.name << "\t\t\t" << e.adres << "\t\t\t" << e.toread << "\t\t\t" << e.towrite << "\t\t\t" << e.sem.value_sem();
	}
	std::cout << std::endl;
}
void Filesystem::displayoft()
{
	std::cout << "NAZWA PLIKU\t\tPROCES" << std::endl;
	for (auto e : openfiletable)
	{
		std::cout << e.first << "\t\t\t" << e.second << std::endl;
	}
}
void Filesystem::displayvector()
{
	for (int i = 0; i < blockTable.size(); i++)
		std::cout << blockTable[i];
}
void Filesystem::displayblock(int block)
{
	std::cout << "|" << block << "|";
	for (int i = block * 32; i < (block + 1) * 32; i++)
	{
		if (maindrive.drivespace[i] < 32 && maindrive.drivespace[i] != 0)
		{
			int f = maindrive.drivespace[i];
			std::cout << f << " ";
		}
		else
		{
			std::cout << maindrive.drivespace[i] << " ";
		}
	}
	std::cout << std::endl;
}
void Filesystem::printqueue(string &filename)
{
	for (int i = 0; i < maincatalogue.size(); i++)//P�TLA SZUKAJ�CA PLIKU W KATALOGU
	{
		if (maincatalogue[i].name == filename)
		{
			maincatalogue[i].sem.print_queue();
			break;
		}
	}
}