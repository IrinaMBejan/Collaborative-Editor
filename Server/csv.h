#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class CSVRow
{
public:
  CSVRow(){};
  const std::vector<std::string>& Get() const;
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


const std::vector<std::string>& CSVRow::Get() const
{
  return rowData;
}

void CSVRow::Add(std::string cell)
{
  rowData.push_back(cell);
}

void CSVWriter::AddRow(const CSVRow& row, bool trunc)
{
  std::fstream file;

  file.open(filename, std::ios::out | (trunc? std::ios::trunc : std::ios::app));
  
  const std::vector<std::string>& to_write = row.Get();

  for (int idx = 0; idx < to_write.size(); idx++)
  {
    file << to_write[idx];
    if ( idx+1 < to_write.size())
      file << ',';
  }
  file << '\n';

  file.close();
}

std::vector<CSVRow> CSVReader::GetData()
{
  std::vector<CSVRow> data;
  std::ifstream file(filename);

  std::string row;
  while(std::getline(file, row))
  {
    std::stringstream rowStream(row);
    std::string cell;
    CSVRow csvRow;
    while (std::getline(rowStream, cell, ','))
    {
      csvRow.Add(cell);
    }
    data.push_back(csvRow);
  }

  return data;
}


