#include "Datasets/nativeVTKDataset.h"
#include "Datasets/VTKDataset.h"
#include "VTKParser.h"
#include "utils.h"
#include <memory>
#include <dirent.h>

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeInitPtr(JNIEnv* env, jobject instance, jlong parserPtr, jlongArray ptFieldValues, jlongArray cellFieldValues)
{
    std::shared_ptr<VTKParser>* parser = (std::shared_ptr<VTKParser>*)(parserPtr);
    std::shared_ptr<VTKDataset>* vtk = new std::shared_ptr<VTKDataset>(new VTKDataset(*parser, jlongArrayToVector<const VTKFieldValue>(env, ptFieldValues),
                                                                       jlongArrayToVector<const VTKFieldValue>(env, cellFieldValues)));


    std::string path     = (*parser)->getPath();
    size_t lastSlash = path.rfind('/');
    std::string dirPath  = ".";
    if(lastSlash != std::string::npos)
        dirPath = path.substr(0, path.rfind('/')+1);
    std::string fileName = path.substr(path.rfind('/')+1);

    DIR* dir = opendir(dirPath.c_str());
    struct dirent* curFile;

    //Search for other VTK subfiles part of this serie (time serie data)
    std::vector<std::string> suffixes;
    while((curFile = readdir(dir)) != NULL)
    {
        std::string timePath = curFile->d_name;
        if(timePath.rfind(fileName, 0) == 0 && timePath.size() > fileName.size()+1)
        {
            std::string suffix = timePath.substr(fileName.size()+1);
            for(char c : suffix)
            {
                if(c < '0' || c > '9')
                    goto endFor;
            }
            suffixes.push_back(suffix);
endFor:;
        }
    }

    //Sort the suffixes
    if(suffixes.size())
    {
        std::sort(suffixes.begin(), suffixes.end());
        for(const auto& s : suffixes)
        {
            VTKParser* suffixParser = new VTKParser((*parser)->getPath() + "." + s);
            if(!suffixParser->parse())
            {
                LOG_ERROR("Could not parse the VTK Dataset %s \n", (fileName + "." + s).c_str());
                delete suffixParser;
                continue;
            }

            std::shared_ptr<VTKParser> sharedSuffixParser(suffixParser);
            (*vtk)->addTimestep(sharedSuffixParser);
        }
    }

    closedir(dir);
    return (jlong)(vtk);
}

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeGetPtFieldValueIndice(JNIEnv* env, jobject instance, jlong vtkPtr, jlong valuePtr)
{
    std::shared_ptr<VTKDataset>* d = (std::shared_ptr<VTKDataset>*)vtkPtr;
    return (*d)->getPtFieldValueIndice((const VTKFieldValue*)valuePtr);
}

JNIEXPORT jint JNICALL Java_com_sereno_vfv_Data_VTKDataset_nativeGetCellFieldValueIndice(JNIEnv* env, jobject instance, jlong vtkPtr, jlong valuePtr)
{
    std::shared_ptr<VTKDataset>* d = (std::shared_ptr<VTKDataset>*)vtkPtr;
    return (*d)->getCellFieldValueIndice((const VTKFieldValue*)valuePtr);
}