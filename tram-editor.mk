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
Date                   :=27/09/2023
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
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)C:\Users\Poga\Desktop\miskaste\wxwidgets\include $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\libraries\bullet $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\libraries\glad $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\libraries $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-sdk\src $(IncludeSwitch)C:\Users\Poga\Desktop\painis\tram-editor\src $(IncludeSwitch)C:\Users\Poga\Desktop\painis\jam-game-ii\src $(IncludeSwitch)C:\Users\Poga\Desktop\painis\jam-game-ii\src\entities 
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
Objects0=$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_language.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_dialogs.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_worldtree.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_viewport.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_editor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_tramsdk.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_settings.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_init.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_mainframe.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_objectmenu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_objects_world.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_widgets_objectlist.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_objects_path.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_editor_objects_entity.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(ObjectSuffix): src/editor/objects/worldcell.cpp $(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/worldcell.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(DependSuffix): src/editor/objects/worldcell.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(DependSuffix) -MM src/editor/objects/worldcell.cpp

$(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(PreprocessSuffix): src/editor/objects/worldcell.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_worldcell.cpp$(PreprocessSuffix) src/editor/objects/worldcell.cpp

$(IntermediateDirectory)/src_editor_language.cpp$(ObjectSuffix): src/editor/language.cpp $(IntermediateDirectory)/src_editor_language.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/language.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_language.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_language.cpp$(DependSuffix): src/editor/language.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_language.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_language.cpp$(DependSuffix) -MM src/editor/language.cpp

$(IntermediateDirectory)/src_editor_language.cpp$(PreprocessSuffix): src/editor/language.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_language.cpp$(PreprocessSuffix) src/editor/language.cpp

$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(ObjectSuffix): src/editor/objects/worldcellmanager.cpp $(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/worldcellmanager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(DependSuffix): src/editor/objects/worldcellmanager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(DependSuffix) -MM src/editor/objects/worldcellmanager.cpp

$(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(PreprocessSuffix): src/editor/objects/worldcellmanager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_worldcellmanager.cpp$(PreprocessSuffix) src/editor/objects/worldcellmanager.cpp

$(IntermediateDirectory)/src_widgets_dialogs.cpp$(ObjectSuffix): src/widgets/dialogs.cpp $(IntermediateDirectory)/src_widgets_dialogs.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/dialogs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_dialogs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_dialogs.cpp$(DependSuffix): src/widgets/dialogs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_dialogs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_dialogs.cpp$(DependSuffix) -MM src/widgets/dialogs.cpp

$(IntermediateDirectory)/src_widgets_dialogs.cpp$(PreprocessSuffix): src/widgets/dialogs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_dialogs.cpp$(PreprocessSuffix) src/widgets/dialogs.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_widgets_worldtree.cpp$(ObjectSuffix): src/widgets/worldtree.cpp $(IntermediateDirectory)/src_widgets_worldtree.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/worldtree.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_worldtree.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_worldtree.cpp$(DependSuffix): src/widgets/worldtree.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_worldtree.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_worldtree.cpp$(DependSuffix) -MM src/widgets/worldtree.cpp

$(IntermediateDirectory)/src_widgets_worldtree.cpp$(PreprocessSuffix): src/widgets/worldtree.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_worldtree.cpp$(PreprocessSuffix) src/widgets/worldtree.cpp

$(IntermediateDirectory)/src_widgets_viewport.cpp$(ObjectSuffix): src/widgets/viewport.cpp $(IntermediateDirectory)/src_widgets_viewport.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/viewport.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_viewport.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_viewport.cpp$(DependSuffix): src/widgets/viewport.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_viewport.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_viewport.cpp$(DependSuffix) -MM src/widgets/viewport.cpp

$(IntermediateDirectory)/src_widgets_viewport.cpp$(PreprocessSuffix): src/widgets/viewport.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_viewport.cpp$(PreprocessSuffix) src/widgets/viewport.cpp

$(IntermediateDirectory)/src_editor_editor.cpp$(ObjectSuffix): src/editor/editor.cpp $(IntermediateDirectory)/src_editor_editor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/editor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_editor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_editor.cpp$(DependSuffix): src/editor/editor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_editor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_editor.cpp$(DependSuffix) -MM src/editor/editor.cpp

$(IntermediateDirectory)/src_editor_editor.cpp$(PreprocessSuffix): src/editor/editor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_editor.cpp$(PreprocessSuffix) src/editor/editor.cpp

$(IntermediateDirectory)/src_tramsdk.cpp$(ObjectSuffix): src/tramsdk.cpp $(IntermediateDirectory)/src_tramsdk.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/tramsdk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_tramsdk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_tramsdk.cpp$(DependSuffix): src/tramsdk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_tramsdk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_tramsdk.cpp$(DependSuffix) -MM src/tramsdk.cpp

$(IntermediateDirectory)/src_tramsdk.cpp$(PreprocessSuffix): src/tramsdk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_tramsdk.cpp$(PreprocessSuffix) src/tramsdk.cpp

$(IntermediateDirectory)/src_editor_settings.cpp$(ObjectSuffix): src/editor/settings.cpp $(IntermediateDirectory)/src_editor_settings.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/settings.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_settings.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_settings.cpp$(DependSuffix): src/editor/settings.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_settings.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_settings.cpp$(DependSuffix) -MM src/editor/settings.cpp

$(IntermediateDirectory)/src_editor_settings.cpp$(PreprocessSuffix): src/editor/settings.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_settings.cpp$(PreprocessSuffix) src/editor/settings.cpp

$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(ObjectSuffix): src/widgets/propertypanel.cpp $(IntermediateDirectory)/src_widgets_propertypanel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/propertypanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(DependSuffix): src/widgets/propertypanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(DependSuffix) -MM src/widgets/propertypanel.cpp

$(IntermediateDirectory)/src_widgets_propertypanel.cpp$(PreprocessSuffix): src/widgets/propertypanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_propertypanel.cpp$(PreprocessSuffix) src/widgets/propertypanel.cpp

$(IntermediateDirectory)/src_init.cpp$(ObjectSuffix): src/init.cpp $(IntermediateDirectory)/src_init.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/init.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_init.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_init.cpp$(DependSuffix): src/init.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_init.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_init.cpp$(DependSuffix) -MM src/init.cpp

$(IntermediateDirectory)/src_init.cpp$(PreprocessSuffix): src/init.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_init.cpp$(PreprocessSuffix) src/init.cpp

$(IntermediateDirectory)/src_widgets_mainframe.cpp$(ObjectSuffix): src/widgets/mainframe.cpp $(IntermediateDirectory)/src_widgets_mainframe.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/mainframe.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_mainframe.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_mainframe.cpp$(DependSuffix): src/widgets/mainframe.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_mainframe.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_mainframe.cpp$(DependSuffix) -MM src/widgets/mainframe.cpp

$(IntermediateDirectory)/src_widgets_mainframe.cpp$(PreprocessSuffix): src/widgets/mainframe.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_mainframe.cpp$(PreprocessSuffix) src/widgets/mainframe.cpp

$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(ObjectSuffix): src/widgets/objectmenu.cpp $(IntermediateDirectory)/src_widgets_objectmenu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/objectmenu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(DependSuffix): src/widgets/objectmenu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(DependSuffix) -MM src/widgets/objectmenu.cpp

$(IntermediateDirectory)/src_widgets_objectmenu.cpp$(PreprocessSuffix): src/widgets/objectmenu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_objectmenu.cpp$(PreprocessSuffix) src/widgets/objectmenu.cpp

$(IntermediateDirectory)/src_editor_objects_world.cpp$(ObjectSuffix): src/editor/objects/world.cpp $(IntermediateDirectory)/src_editor_objects_world.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/world.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_world.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_world.cpp$(DependSuffix): src/editor/objects/world.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_world.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_world.cpp$(DependSuffix) -MM src/editor/objects/world.cpp

$(IntermediateDirectory)/src_editor_objects_world.cpp$(PreprocessSuffix): src/editor/objects/world.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_world.cpp$(PreprocessSuffix) src/editor/objects/world.cpp

$(IntermediateDirectory)/src_widgets_objectlist.cpp$(ObjectSuffix): src/widgets/objectlist.cpp $(IntermediateDirectory)/src_widgets_objectlist.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/widgets/objectlist.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widgets_objectlist.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widgets_objectlist.cpp$(DependSuffix): src/widgets/objectlist.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widgets_objectlist.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widgets_objectlist.cpp$(DependSuffix) -MM src/widgets/objectlist.cpp

$(IntermediateDirectory)/src_widgets_objectlist.cpp$(PreprocessSuffix): src/widgets/objectlist.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widgets_objectlist.cpp$(PreprocessSuffix) src/widgets/objectlist.cpp

$(IntermediateDirectory)/src_editor_objects_path.cpp$(ObjectSuffix): src/editor/objects/path.cpp $(IntermediateDirectory)/src_editor_objects_path.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/path.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_path.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_path.cpp$(DependSuffix): src/editor/objects/path.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_path.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_path.cpp$(DependSuffix) -MM src/editor/objects/path.cpp

$(IntermediateDirectory)/src_editor_objects_path.cpp$(PreprocessSuffix): src/editor/objects/path.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_path.cpp$(PreprocessSuffix) src/editor/objects/path.cpp

$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(ObjectSuffix): src/editor/objects/navmesh.cpp $(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/navmesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(DependSuffix): src/editor/objects/navmesh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(DependSuffix) -MM src/editor/objects/navmesh.cpp

$(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(PreprocessSuffix): src/editor/objects/navmesh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_navmesh.cpp$(PreprocessSuffix) src/editor/objects/navmesh.cpp

$(IntermediateDirectory)/src_editor_objects_entity.cpp$(ObjectSuffix): src/editor/objects/entity.cpp $(IntermediateDirectory)/src_editor_objects_entity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-editor/src/editor/objects/entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_editor_objects_entity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_editor_objects_entity.cpp$(DependSuffix): src/editor/objects/entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_editor_objects_entity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_editor_objects_entity.cpp$(DependSuffix) -MM src/editor/objects/entity.cpp

$(IntermediateDirectory)/src_editor_objects_entity.cpp$(PreprocessSuffix): src/editor/objects/entity.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_editor_objects_entity.cpp$(PreprocessSuffix) src/editor/objects/entity.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


