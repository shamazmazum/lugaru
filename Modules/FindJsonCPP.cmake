FIND_PATH(JSONCPP_INCLUDE_DIR NAMES
  json/allocator.h
  json/assertions.h
  json/autolink.h
  json/config.h
  json/forwards.h
  json/json.h
  json/json_features.h
  json/reader.h
  json/value.h
  json/version.h
  json/writer.h)
MARK_AS_ADVANCED(JSONCPP_INCLUDE_DIR)
 
FIND_LIBRARY(JSONCPP_LIBRARIES NAMES
  jsoncpp
  libjsoncpp
  jsoncpplib)
MARK_AS_ADVANCED(JSONCPP_LIBRARIES)
 
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (JsonCPP DEFAULT_MSG
                                   JSONCPP_LIBRARIES JSONCPP_INCLUDE_DIR)
