#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>

class CSVRow
{
public:
  CSVRow(){};
  inline const std::vector<std::string>& Get() const {return rowData;}
  inline const std::string& GetAt(int idx) const {return rowData[idx];}
  void Add(std::string cell);

private:
  std::vector<std::string> rowData;

};

class CSVReader
{
  std::string filename;

public:
  CSVReader(std::string filename) : filename(filename){};
  std::vector<CSVRow> GetData();

private:
  std::vector<CSVRow> data;

};

class CSVWriter
{
  std::string filename;
public:
  CSVWriter(std::string filename): filename(filename){};
  void AddRow(const CSVRow& row, bool trunc=false);
};

#endif // CSV_H
