//
// Created by liu on 2016/12/21.
//

#pragma once

#include <QColor>
#include <QTextStream>

#include "../../core/core.h"
#include "../../core/ui/all.h"
#include "../../core/package/Package.h"
#include "../../core/Project.h"
#include "../../components/property_browser/Browser.h"
#include "../../components/property_browser/Editor.h"

//using namespace TCUIEdit::core;

namespace TCUIEdit { namespace mainview
{
    class Base : public QObject
    {
    Q_OBJECT
    protected:
        property_browser::Browser *m_browser;
        core::ui::Base *m_uiBase;
    public:
        Base(property_browser::Browser *browser, core::ui::Base *ui);

        virtual void refresh();

        virtual void examineName(property_browser::Row *row);

        virtual void updateDisplay(property_browser::Row *row);

    protected slots:

        virtual void onNameEdited(TCUIEdit::property_browser::Row *row);

        virtual void onDisplayEdited(TCUIEdit::property_browser::Row *row);

    };
}}



