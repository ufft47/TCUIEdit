//
// Created by sunyi on 2016/12/29.
//

#pragma once

#include "Base.h"
#include "../../core/ui/Param.h"

namespace TCUIEdit { namespace mainview
{
    class Param : public Base
    {
    Q_OBJECT
    protected:
        core::ui::Param *m_ui;
    public:
        Param(TCUIEdit::property_browser::Browser *browser, core::ui::Base *ui);

        void refresh();
    };
}}
