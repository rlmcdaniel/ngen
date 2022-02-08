#include "test_bmi_cpp.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <math.h>

std::string TestBmiCpp::GetComponentName(){
  return "Testing BMI C++ Model";
}

double TestBmiCpp::GetCurrentTime(){
  return current_model_time;
}

double TestBmiCpp::GetEndTime(){
  double time = this->GetStartTime();
  time += this->num_time_steps * this->time_step_size;
  return time;
}

int TestBmiCpp::GetGridRank(const int grid){
  if (grid == 0) {
      return 1;
  }
  else {
    throw std::runtime_error("Rank requested for non-existent grid.");
  }
}

int TestBmiCpp::GetGridSize(const int grid){
  if (grid == 0) {
      return 1;
  }
  else {
    throw std::runtime_error("Size requested for non-existent grid.");
  }
}

std::string TestBmiCpp::GetGridType(const int grid){
  if (grid == 0) {
      return "scalar";
  }
  else {
    throw std::runtime_error("Type requested for non-existent grid.");
  }
}


std::vector<std::string> TestBmiCpp::GetInputVarNames(){
  return input_var_names;  
}
std::vector<std::string> TestBmiCpp::GetOutputVarNames(){
  return output_var_names;
}
int TestBmiCpp::GetInputItemCount(){
  return input_var_names.size();
}
int TestBmiCpp::GetOutputItemCount(){
  return output_var_names.size();
}

double TestBmiCpp::GetStartTime(){
  return 0.0;
}

double TestBmiCpp::GetTimeStep(){
  return this->time_step_size;
}

std::string TestBmiCpp::GetTimeUnits(){
  return "s";
}

void TestBmiCpp::GetValue(std::string name, void* dest){
  int inds[] = {0};
  this->GetValueAtIndices(name, dest, inds, 1);
}

void TestBmiCpp::GetValueAtIndices(std::string name, void* dest, int* inds, int len){
  if (len < 1)
    throw std::runtime_error(std::string("Illegal count ") + std::to_string(len) + std::string(" provided to SetValueAtIndices(name, dest, inds, count)"));

  void *ptr;
  std::string type;
  ptr = this->GetValuePtr(name);
  type = this->GetVarType(name);

  if(type == BMI_TYPE_NAME_DOUBLE){
    for (size_t i = 0; i < len; ++i) {
        ((double*)dest)[i] = ((double*)ptr)[inds[i]];
    }
    return;
  }

  if (type == BMI_TYPE_NAME_INT) {
    for (size_t i = 0; i < len; ++i) {
      ((int*)dest)[i] = ((int*)ptr)[inds[i]];
    }
  }

  if (type == BMI_TYPE_NAME_FLOAT) {
    for (size_t i = 0; i < len; ++i) {
      ((float*)dest)[i] = ((float*)ptr)[inds[i]];
    }
    return;
  }

  if (type == BMI_TYPE_NAME_LONG) {
    for (size_t i = 0; i < len; ++i) {
      ((long*)dest)[i] = ((long*)ptr)[inds[i]];
    }
    return;
  }

}

void* TestBmiCpp::GetValuePtr(std::string name){
  if (name == "INPUT_VAR_1") {
    return this->input_var_1.get();
  }
  if (name == "INPUT_VAR_2") {
    return this->input_var_2.get();
  }
  if (name == "OUTPUT_VAR_1") {
    return this->output_var_1.get();
  }
  if (name == "OUTPUT_VAR_2") {
    return this->output_var_2.get();
  }
  throw std::runtime_error("GetValuePtr called for nknown variable: "+name);
}

int TestBmiCpp::GetVarItemsize(std::string name){
  std::map<std::string,int>::const_iterator iter = this->type_sizes.find(this->GetVarType(name));
  if(iter != this->type_sizes.end()){
    return iter->second;
  }
  throw std::runtime_error("Item \""+name+"\" has illegal type \""+(this->GetVarType(name))+"\"!");
}

std::string TestBmiCpp::GetVarLocation(std::string name){
  auto iter = std::find(this->output_var_names.begin(), this->output_var_names.end(), name);
  if(iter != this->output_var_names.end()){
    return this->output_var_locations[iter - this->output_var_names.begin()];
  }
  iter = std::find(this->input_var_names.begin(), this->input_var_names.end(), name);
  if(iter != this->input_var_names.end()){
    return this->input_var_locations[iter - this->input_var_names.begin()];
  }
  throw std::runtime_error("GetVarLocation called for non-existent variable: "+name);
}

int TestBmiCpp::GetVarNbytes(std::string name){
  int item_size = this->GetVarItemsize(name);

  // this will never actually get used, but mimicing the C version...
  // if a different item_count isn't found below, the call above will have already thrown.
  int item_count = this->num_time_steps; 

  auto iter = std::find(this->output_var_names.begin(), this->output_var_names.end(), name);
  if(iter != this->output_var_names.end()){
    item_count = this->output_var_item_count[iter - this->output_var_names.begin()];
  }
  iter = std::find(this->input_var_names.begin(), this->input_var_names.end(), name);
  if(iter != this->input_var_names.end()){
    item_count = this->input_var_item_count[iter - this->input_var_names.begin()];
  }
  return item_size * item_count;
}

std::string TestBmiCpp::GetVarType(std::string name){
  auto iter = std::find(this->output_var_names.begin(), this->output_var_names.end(), name);
  if(iter != this->output_var_names.end()){
    return this->output_var_types[iter - this->output_var_names.begin()];
  }
  iter = std::find(this->input_var_names.begin(), this->input_var_names.end(), name);
  if(iter != this->input_var_names.end()){
    return this->input_var_types[iter - this->input_var_names.begin()];
  }
  throw std::runtime_error("GetVarType called for non-existent variable: "+name);
}

std::string TestBmiCpp::GetVarUnits(std::string name){
  auto iter = std::find(this->output_var_names.begin(), this->output_var_names.end(), name);
  if(iter != this->output_var_names.end()){
    return this->output_var_units[iter - this->output_var_names.begin()];
  }
  iter = std::find(this->input_var_names.begin(), this->input_var_names.end(), name);
  if(iter != this->input_var_names.end()){
    return this->input_var_units[iter - this->input_var_names.begin()];
  }
  throw std::runtime_error("GetVarUnits called for non-existent variable: "+name);
}

void TestBmiCpp::Initialize(std::string file){
  if (file == "")
    throw std::runtime_error("No configuration file path provided.");

  this->read_init_config(file);

  this->current_model_time = this->GetStartTime();

  // If neither of these is read from config (remain 0 as initialized), fall back to default for ts count
  if (this->num_time_steps == 0 && this->model_end_time == 0) {
      this->num_time_steps = DEFAULT_TIME_STEP_COUNT;
  }
  // Now at least one must be set
  assert(this->model_end_time != 0 || this->num_time_steps != 0);
  // Whenever end time is not already set here, derive based on num_time_steps
  if (this->model_end_time == 0) {
      assert(this->num_time_steps != 0);
      this->model_end_time = this->current_model_time + (this->num_time_steps * this->time_step_size);
  }
  assert(this->model_end_time != 0);
  if (this->num_time_steps == 0) {
      this->num_time_steps = floor((this->model_end_time - this->current_model_time) / this->time_step_size);
  }

  this->input_var_1 = std::make_unique<double>(0);
  this->input_var_2 = std::make_unique<double>(0);
  this->output_var_1 = std::make_unique<double>(0);
  this->output_var_2 = std::make_unique<double>(0);
}

void TestBmiCpp::SetValueAtIndices(std::string name, int* inds, int len, void* src){
  if (len < 1)
    throw std::runtime_error(std::string("Illegal count ") + std::to_string(len) + std::string(" provided to SetValueAtIndices(name, dest, inds, count)"));

  void* ptr;
  ptr = this->GetValuePtr(name);
  std::string type = this->GetVarType(name);

  if (type == BMI_TYPE_NAME_DOUBLE) {
    for (size_t i = 0; i < len; ++i) {
        ((double*)ptr)[inds[i]] = ((double*)src)[i];
    }
  } else if (type == BMI_TYPE_NAME_INT) {
    for (size_t i = 0; i < len; ++i) {
        ((int*)ptr)[inds[i]] = ((int*)src)[i];
    }
  } else if (type == BMI_TYPE_NAME_FLOAT) {
    for (size_t i = 0; i < len; ++i) {
        ((float*)ptr)[inds[i]] = ((float*)src)[i];
    }
  } else if (type == BMI_TYPE_NAME_LONG) {
    for (size_t i = 0; i < len; ++i) {
        ((long*)ptr)[inds[i]] = ((long*)src)[i];
    }
  }
}

void TestBmiCpp::SetValue(std::string name, void* src){
  void *dest = this->GetValuePtr(name);
  int nbytes = this->GetVarNbytes(name);
  std::memcpy (dest, src, nbytes);
}

void TestBmiCpp::Update(){
  this->UpdateUntil(this->current_model_time + this->time_step_size);
}

void TestBmiCpp::UpdateUntil(double future_time){
  this->run((long)(future_time - this->current_model_time));
  if (this->current_model_time != future_time)
    this->current_model_time = future_time;
}

void TestBmiCpp::Finalize(){
  return;
}

int TestBmiCpp::GetGridEdgeCount(const int grid){
  throw std::logic_error("Not implemented.");
}
int TestBmiCpp::GetGridFaceCount(const int grid){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridEdgeNodes(const int grid, int* edge_nodes){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridFaceEdges(const int grid, int* face_edges){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridFaceNodes(const int grid, int* face_nodes){
  throw std::logic_error("Not implemented.");
}
int TestBmiCpp::GetGridNodeCount(const int grid){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridNodesPerFace(const int grid, int* nodes_per_face){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridOrigin(const int grid, double* origin){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridShape(const int grid, int* shape){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridSpacing(const int grid, double* spacing){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridX(const int grid, double* x){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridY(const int grid, double* y){
  throw std::logic_error("Not implemented.");
}
void TestBmiCpp::GetGridZ(const int grid, double* z){
  throw std::logic_error("Not implemented.");
}
int TestBmiCpp::GetVarGrid(std::string name){
  throw std::logic_error("Not implemented.");
}

void TestBmiCpp::read_init_config(std::string config_file)
{
  int config_line_count, max_config_line_length;
  // Note that this determines max line length including the ending return character, if present
  read_file_line_counts(config_file, &config_line_count, &max_config_line_length);

  FILE* fp = fopen(config_file.c_str(), "r");
  if (fp == NULL)
    throw std::runtime_error("Invalid config file \""+config_file+"\"");

  char config_line[max_config_line_length + 1];

  // TODO: may need to add other variables to track that everything that was required was properly set

  // Keep track of whether required values were set in config
  int is_epoch_start_time_set = FALSE;

  for (size_t i = 0; i < config_line_count; i++) {
    char *param_key, *param_value;
    fgets(config_line, max_config_line_length + 1, fp);

    char* config_line_ptr = config_line;
    config_line_ptr = strsep(&config_line_ptr, "\n");
    param_key = strsep(&config_line_ptr, "=");
    param_value = strsep(&config_line_ptr, "=");

#if DEGUG >= 1
    std::cout<<"Config Value - Param: '"<<param_key<<"' | Value: '"<<param_value<<"'"<<std::endl;
#endif

    if (strcmp(param_key, "epoch_start_time") == 0) {
      this->epoch_start_time = strtol(param_value, NULL, 10);
      is_epoch_start_time_set = TRUE;
      continue;
    }
    if (strcmp(param_key, "num_time_steps") == 0) {
      this->num_time_steps = (int)strtol(param_value, NULL, 10);
      continue;
    }
    if (strcmp(param_key, "time_step_size") == 0) {
      this->time_step_size = (int)strtol(param_value, NULL, 10);
      continue;
    }
    if (strcmp(param_key, "model_end_time") == 0) {
      this->time_step_size = (int)strtol(param_value, NULL, 10);
      continue;
    }
  }

  if (is_epoch_start_time_set == FALSE) {
    throw std::runtime_error("Config param 'epoch_start_time' not found in config file");
  }

#if DEGUG >= 1
  std::cout<<"All TestBmiCpp config params present; finished parsing config"<<std::endl;
#endif

}

void TestBmiCpp::read_file_line_counts(std::string file_name, int* line_count, int* max_line_length)
{
  *line_count = 0;
  *max_line_length = 0;
  int current_line_length = 0;
  FILE* fp = fopen(file_name.c_str(), "r");
  // Ensure exists
  if (fp == NULL) {
    throw std::runtime_error("Configuration file does not exist.");
  }
  int seen_non_whitespace = 0;
  char c;
  for (c = fgetc(fp); c != EOF; c = fgetc(fp)) {
    // keep track if this line has seen any char other than space or tab
    if (c != ' ' && c != '\t' && c != '\n')
      seen_non_whitespace++;
    // Update line count, reset non-whitespace count, adjust max_line_length (if needed), and reset current line count
    if (c == '\n') {
      *line_count += 1;
      seen_non_whitespace = 0;
      if (current_line_length > *max_line_length)
        *max_line_length = current_line_length;
      current_line_length = 0;
    }
    else {
      current_line_length += 1;
    }
  }
  fclose(fp);

  // If we saw some non-whitespace char on last line, assume last line didn't have its own \n, so count needs to be
  // incremented by 1.
  if (seen_non_whitespace > 0) {
    *line_count += 1;
  }

  // Before returning, increment the max line length by 1, since the \n will be on the line also.
  *max_line_length += 1;
}



void TestBmiCpp::run(long dt)
{
    if (dt == this->time_step_size) {
        *this->output_var_1 = *this->input_var_1;
        *this->output_var_2 = 2.0 * *this->input_var_2;
    }
    else {
        *this->output_var_1 = *this->input_var_1 * (double) dt / this->time_step_size;
        *this->output_var_2 = 2.0 * *this->input_var_2 * (double) dt / this->time_step_size;
    }
    this->current_model_time += (double)dt;
}