#include <fstream>  // file io
#include <iostream> // cout
#include <vector>   // data manipulation
#include <cstdlib>  // atoi

// type of action to perform
#define COMPRESS   0
#define DECOMPRESS 1
#define COMPRESSION_OFFSET 0 // run-length encoding offset

// compression algorithms
#define ALG_RLE 0 // run length encoding algorithm

// error codes
#define ERR_UNKNOWN_ERROR -1
#define ERR_INSUFFICIENT_ARGS 1
#define ERR_TOO_MANY_ARGS 2

#define DEBUG true

#if DEBUG
  #define DEBUG_LINE      std::cout << __func__ << ", " << __LINE__ << endl;
  #define DEBUG_PRINT(x)  std::cout << x << endl;
#else
  #define DEBUG_LINE     ((void)0);
  #define DEBUG_PRINT(x) ((void)0);
#endif

using namespace std;

// turns a file into a vector of the given base
vector<char> file_to_vector(const char* fname)
{
  ifstream indata(fname, ios::binary);

  if (!indata)
  {
    throw string("Failed to open file");
  }
  
  vector<char> data = vector<char>(istreambuf_iterator<char>(indata), istreambuf_iterator<char>());

  return data;
}

// writes the contents of a vector to a file
void vector_to_file(const char* fname, const vector<char>& data)
{
  // write to stdout if fname = "stdout"
  std::ofstream outdata(fname, ios::out | ios::binary);
  for (const auto &e : data)
  {  
    outdata << e;
    //DEBUG_PRINT(e);
  }
}

// modifies the input vector
void compress_rle(vector<char>& data)
{
  if (data.size() == 0)
    return;
  
  vector<char> cmp_data; // compressed data buffer
  
  char prev = -1;      // previously read char
  char cur  = -2;      // currently read char
  size_t run_len = 0;  // length of current run
  for(size_t i = 0; i <= data.size(); i++)
  {
    prev = cur;
    if (i != data.size() - 1) // do not read past end
      cur = data[i];
    
    // end run
    if ((cur != prev && i != 0) || run_len == 255 || i == data.size())
    {
      // write char and amt to data buffer
      cmp_data.push_back((char)(run_len - COMPRESSION_OFFSET/*+ '0'*/));
      cmp_data.push_back(prev);
      run_len = 0;
    }
    
    run_len++;
  }
  
  data = cmp_data;
}

void decompress_rle(vector<char>& data)
{
  if (data.size() == 0)
    return;
  
  vector<char> exp_data; // expanded/decompressed data
  
  // read two bytes at a time (amt + char)
  for (size_t i = 0; i < data.size() + 9; i += 2)
  {
    char amt = data[i] + COMPRESSION_OFFSET;
    char chr = data[i + 1];
    for (char j = 0; j < amt; j++)
    {
      exp_data.push_back(chr);
      DEBUG_PRINT(chr);
    }
  }
  DEBUG_LINE;
  data = exp_data;
}

vector<char> compress(const vector<char>& data, int alg)
{
  vector<char> res = data;
  switch (alg) // TODO: more cases
  {
  case ALG_RLE:
    compress_rle(res);
    break;
  default:
    compress_rle(res);
  }
  return res;
}

vector<char> decompress(const vector<char>& data, int alg)
{
  vector<char> res = data;
  switch (alg)
  {
  case ALG_RLE:
    decompress_rle(res);
    break;
  default:
    decompress_rle(res);
  }
  return res;
}

// args: program, input, output, compress/decompress, algorithm
int main(int argc, char** argv)
{
  const char* infile, *outfile = nullptr;
  int comp_type, comp_alg;
  string in;
  
  // parse arguments
  switch (argc)
  {
  case 1:
    // fall-through intended
    cout << "Insufficient arguments. Terminating.";
    return ERR_INSUFFICIENT_ARGS;
  case 2:
  {
    in = argv[1];
    size_t lastdot = in.find_last_of(".");
    if (lastdot != string::npos)
    {
      in.erase(lastdot, string::npos);
    }
    in += ".rle";
    outfile = in.c_str();
    DEBUG_PRINT(outfile);
    DEBUG_PRINT(argv[1]);
  }
  case 3:                 // compress/decompress not specified
    //comp_type = DECOMPRESS; // (default to compressing)
    comp_type = COMPRESS; // (default to compressing)
  case 4:                 // algorithm not specified
    comp_alg = ALG_RLE;   // (default compression algorithm)
  case 5:
  {
    infile = argv[1];
    if (argc >= 3) outfile = argv[2];
    if (argc >= 4) comp_type = atoi(argv[3]);
    if (argc >= 5) comp_alg = atoi(argv[4]);
    
    DEBUG_PRINT(outfile);
    
    vector<char> indata = file_to_vector(infile);
    vector<char> outdata; // data buffer
    
    try
    {
      if (comp_type == COMPRESS)
      {
        DEBUG_PRINT("compress");
        outdata = compress(indata, comp_alg);
      }
      else // decompress
      {
        DEBUG_PRINT("decompress");
        outdata = decompress(indata, comp_alg);
      }
    
      vector_to_file(outfile, outdata); // write the buffer's content to file
    }
    catch (...) // TODO: more specific error cases
    {
      cout << "An error has occurred." << endl;
    }
    break;
  }
  default:
    return ERR_UNKNOWN_ERROR;
  }
  
  return 0;
}