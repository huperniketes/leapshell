#include "StdAfx.h"
#include "Request.h"
#include <fstream>
//
//Request::Request() : m_progress(0), m_abort(false), m_readOffset(0) {
//  m_curl = curl_easy_init();
//}
//
//Request::~Request() {
//  curl_easy_cleanup(m_curl);
//}
//
//bool Request::Upload(const std::string& filepath, const std::string& filename) {
//  // set callbacks
//  curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, readContentStatic);
//  curl_easy_setopt(m_curl, CURLOPT_READDATA, this);
//  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeContentStatic);
//  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
//  curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, progressFunctionStatic);
//  curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, this);
//
//  // set options
//  curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 0L);
//  curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);
//  curl_easy_setopt(m_curl, CURLOPT_PUT, 1L);
//
//  // load file
//  std::ifstream fileInput(filepath.c_str());
//  if (!fileInput) {
//    return false;
//  }
//  m_data = std::string((std::istreambuf_iterator<char>(fileInput)), std::istreambuf_iterator<char>());
//  fileInput.close();
//
//  // perform upload
//  m_progress = 0;
//  m_abort = false;
//  m_readOffset = 0;
//  const size_t numBytes = m_data.size();
//  std::string url = createUploadURL("freeform", filename, numBytes);
//  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
//  curl_easy_setopt(m_curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)numBytes);
//  CURLcode result = curl_easy_perform(m_curl);
//  return (result == CURLE_OK);
//}
//
//void Request::LaunchForm(const std::string& filename, const std::string& name, const std::string& description) {
//  const std::string url = "http://leapmotion-freeform.herokuapp.com/sculpteo/aws_to_store/freeform/" + filename + "?name=" + name + "&description=" + description;
//  ci::launchWebBrowser(ci::Url(url));
//}
//
//void Request::Init() {
//  curl_global_init(CURL_GLOBAL_DEFAULT);
//}
//
//void Request::Cleanup() {
//  curl_global_cleanup();
//}
//
//size_t Request::readContent(char* ptr, size_t size, size_t nmemb) {
//  const size_t bytesAvailable = (m_data.size() - m_readOffset);
//  const size_t numBytes = std::min(bytesAvailable, size*nmemb);
//  memcpy(ptr, m_data.data() + m_readOffset, numBytes);
//  m_readOffset += numBytes;
//  return numBytes;
//}
//
//size_t Request::writeContent(char* ptr, size_t size, size_t nmemb) {
//  const size_t numBytes = size*nmemb;
//  m_response = std::string(ptr, numBytes);
//  return numBytes;
//}
//
//int Request::progressFunction(double dltotal, double dlnow, double ultotal, double ulnow) {
//  m_progress = ulnow / ultotal;
//  return static_cast<int>(m_abort);
//}
//
//size_t Request::readContentStatic(char* ptr, size_t size, size_t nmemb, void* userdata) {
//  return reinterpret_cast<Request*>(userdata)->readContent(ptr, size, nmemb);
//}
//
//size_t Request::writeContentStatic(char* ptr, size_t size, size_t nmemb, void* userdata) {
//  return reinterpret_cast<Request*>(userdata)->writeContent(ptr, size, nmemb);
//}
//
//int Request::progressFunctionStatic(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
//  return reinterpret_cast<Request*>(clientp)->progressFunction(dltotal, dlnow, ultotal, ulnow);
//}
//
//std::string Request::createUploadURL(const std::string& username, const std::string& filename, size_t numBytes) {
//  std::stringstream ss;
//  ss << "http://leapmotion-freeform.herokuapp.com/item/";
//  ss << username << "/" << filename << "?size=" << numBytes;
//  return ss.str();
//}
