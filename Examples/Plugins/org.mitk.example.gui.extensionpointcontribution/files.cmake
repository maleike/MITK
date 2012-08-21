set(SRC_CPP_FILES
)

set(INTERNAL_CPP_FILES
  org_mitk_example_gui_extensionpointcontribution_Activator.cpp
  ExtensionPointContribution.cpp
  MinimalPerspective.cpp
  MinimalView.cpp
  IChangeText.cpp
)

set(UI_FILES
  src/internal/MinimalViewControls.ui
)

set(MOC_H_FILES
  src/internal/org_mitk_example_gui_extensionpointcontribution_Activator.h

  src/internal/ExtensionPointContribution.h
  src/internal/MinimalPerspective.h
  src/internal/MinimalView.h
  src/internal/IChangeText.h
)

set(CACHED_RESOURCE_FILES
  plugin.xml
)

set(CPP_FILES )

foreach(file ${SRC_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/${file})
endforeach(file ${SRC_CPP_FILES})

foreach(file ${INTERNAL_CPP_FILES})
  set(CPP_FILES ${CPP_FILES} src/internal/${file})
endforeach(file ${INTERNAL_CPP_FILES})
