//
// Created by liu on 2016/12/8.
//

#ifndef TCUIEDIT_UIBASE_TYPE_H
#define TCUIEDIT_UIBASE_TYPE_H


#include "../UICore.h"
#include "UIBase.h"

namespace TCUIEdit
{
    //***************************************************************************
    // [TriggerTypes]
    // Defines all trigger variable types to be used by the Script Editor
    // Key: type name

    // Value 0: first game version in which this type is valid
    // Value 1: flag (0 or 1) indicating if this type can be a global variable
    // Value 2: flag (0 or 1) indicating if this type can be used with comparison operators
    // Value 3: string to display in the editor
    // Value 4: base type, used only for custom types
    // Value 5: import type, for strings which represent files (optional)
    // Value 6: flag (0 or 1) indicating to treat this type as the base type in the editor

    class UIBase_Type : public UIBase
    {

    protected:
        QString icon, displayFlag;
    public:
        UIBase_Type(UIPackage *package, QPair<QString, QStringList> pair);
        const QString getDisplayName() const;
    };
};

#endif //TCUIEDIT_UIBASE_TYPE_H
