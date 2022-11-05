##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=tram-editor
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=C:/Users/Poga/Desktop/painis/tram
ProjectPath            :=C:/Users/Poga/Desktop/painis/tram-editor
IntermediateDirectory  :=../tram/build-$(WorkspaceConfiguration)/__/tram-editor
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Poga
Date                   :=05/11/2022
CodeLitePath           :="C:/Program Files/CodeLite"
MakeDirCommand         :=mkdir
LinkerName             :=C:/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=C:/Users/Poga/Desktop/painis/tram/build-$(WorkspaceConfiguration)/bin
OutputFile             :=..\tram\build-$(WorkspaceConfiguration)\bin\$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=C:/mingw64/bin/windres.exe
LinkOptions            :=  -mwindows -pipe -lwxmsw31u_gl -lwxmsw31u_propgrid -lwxmsw31u_aui -lwxmsw31u_xrc -lwxmsw31u_webview -lwxmsw31u_html -lwxmsw31u_adv -lwxmsw31u_core -lwxbase31u_xml -lwxbase31u_net -lwxbase31u
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)C:\Users\Poga\Desktop\miskaste\wxwidgets\include $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\libraries $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\src $(IncludeSwitch)C:\Users\Poga\Desktop\painis\jam-game\src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:\Users\Poga\Desktop\miskaste\wxwidgets\lib\gcc_dll 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/mingw64/bin/ar.exe rcu
CXX      := C:/mingw64/bin/g++.exe
CC       := C:/mingw64/bin/gcc.exe
CXXFLAGS :=  -g -O0 -Wall $(shell wx-config --cxxflags --unicode=yes  ) -std=c++20 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(shell wx-config --cxxflags --unicode=yes  ) $(Preprocessors)
ASFLAGS  := 
AS       := C:/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\Users\Poga\Desktop\miskaste\wxwidgets
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/language.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/editor.cpp$(ObjectSuffix) $(IntermediateDirectory)/viewport.cpp$(ObjectSuffix) $(IntermediateDirectory)/objectlist.cpp$(ObjectSuffix) $(IntermediateDirectory)/objectmenu.cpp$(ObjectSuffix) $(IntermediateDirectory)/propertypanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/worldtree.cpp$(ObjectSuffix) $(IntermediateDirectory)/benis.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@if not exist "$(OutputDirectory)" $(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/language.cpp$(ObjectSuffix): language.cpp $(IntermediateDirectory)/language.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/language.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/language.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/language.cpp$(DependSuffix): language.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/language.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/language.cpp$(DependSuffix) -MM language.cpp

$(IntermediateDirectory)/language.cpp$(PreprocessSuffix): language.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/language.cpp$(PreprocessSuffix) language.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/editor.cpp$(ObjectSuffix): editor.cpp $(IntermediateDirectory)/editor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/editor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/editor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/editor.cpp$(DependSuffix): editor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/editor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/editor.cpp$(DependSuffix) -MM editor.cpp

$(IntermediateDirectory)/editor.cpp$(PreprocessSuffix): editor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/editor.cpp$(PreprocessSuffix) editor.cpp

$(IntermediateDirectory)/viewport.cpp$(ObjectSuffix): viewport.cpp $(IntermediateDirectory)/viewport.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/viewport.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/viewport.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/viewport.cpp$(DependSuffix): viewport.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/viewport.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/viewport.cpp$(DependSuffix) -MM viewport.cpp

$(IntermediateDirectory)/viewport.cpp$(PreprocessSuffix): viewport.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/viewport.cpp$(PreprocessSuffix) viewport.cpp

$(IntermediateDirectory)/objectlist.cpp$(ObjectSuffix): objectlist.cpp $(IntermediateDirectory)/objectlist.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/objectlist.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/objectlist.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/objectlist.cpp$(DependSuffix): objectlist.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/objectlist.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/objectlist.cpp$(DependSuffix) -MM objectlist.cpp

$(IntermediateDirectory)/objectlist.cpp$(PreprocessSuffix): objectlist.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/objectlist.cpp$(PreprocessSuffix) objectlist.cpp

$(IntermediateDirectory)/objectmenu.cpp$(ObjectSuffix): objectmenu.cpp $(IntermediateDirectory)/objectmenu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/objectmenu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/objectmenu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/objectmenu.cpp$(DependSuffix): objectmenu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/objectmenu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/objectmenu.cpp$(DependSuffix) -MM objectmenu.cpp

$(IntermediateDirectory)/objectmenu.cpp$(PreprocessSuffix): objectmenu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/objectmenu.cpp$(PreprocessSuffix) objectmenu.cpp

$(IntermediateDirectory)/propertypanel.cpp$(ObjectSuffix): propertypanel.cpp $(IntermediateDirectory)/propertypanel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/propertypanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/propertypanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/propertypanel.cpp$(DependSuffix): propertypanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/propertypanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/propertypanel.cpp$(DependSuffix) -MM propertypanel.cpp

$(IntermediateDirectory)/propertypanel.cpp$(PreprocessSuffix): propertypanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/propertypanel.cpp$(PreprocessSuffix) propertypanel.cpp

$(IntermediateDirectory)/worldtree.cpp$(ObjectSuffix): worldtree.cpp $(IntermediateDirectory)/worldtree.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/worldtree.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/worldtree.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/worldtree.cpp$(DependSuffix): worldtree.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/worldtree.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/worldtree.cpp$(DependSuffix) -MM worldtree.cpp

$(IntermediateDirectory)/worldtree.cpp$(PreprocessSuffix): worldtree.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/worldtree.cpp$(PreprocessSuffix) worldtree.cpp

$(IntermediateDirectory)/benis.cpp$(ObjectSuffix): benis.cpp $(IntermediateDirectory)/benis.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/benis.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/benis.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/benis.cpp$(DependSuffix): benis.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/benis.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/benis.cpp$(DependSuffix) -MM benis.cpp

$(IntermediateDirectory)/benis.cpp$(PreprocessSuffix): benis.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/benis.cpp$(PreprocessSuffix) benis.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


