/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2010, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "clearExpressionField.H"

#include "FieldValueExpressionDriver.H"

#include "expressionFieldFunctionObject.H"

namespace Foam {
    defineTypeNameAndDebug(clearExpressionField,0);
}

Foam::clearExpressionField::clearExpressionField
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    obr_(obr)
{
    read(dict);
}

Foam::clearExpressionField::~clearExpressionField()
{}

void Foam::clearExpressionField::timeSet()
{
    // Do nothing
}

void Foam::clearExpressionField::read(const dictionary& dict)
{
    name_=word(dict.lookup("fieldName"));
}

void Foam::clearExpressionField::execute()
{
    const functionObjectList &fol=obr_.time().functionObjects();
    bool found=false;

    forAll(fol,i) {
        if(isA<expressionFieldFunctionObject>(fol[i])) {
            expressionField &ef=const_cast<expressionField &>(
                //                dynamicCast<const expressionFieldFunctionObject&>(fol[i]).outputFilter() // doesn't work with gcc 4.2
                dynamic_cast<const expressionFieldFunctionObject&>(fol[i]).outputFilter()
            );

            if(ef.name()==name_) {
                found=true;
                ef.clearData();
            }
        }
    }

    if(!found) {
        WarningIn("clearExpressionField::execute()")
            << "No function object named " << name_ << " found"
                << endl;
    }
}


void Foam::clearExpressionField::end()
{
}

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
bool
#else
void
#endif
Foam::clearExpressionField::write()
{
#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    return true;
#endif
}

// ************************************************************************* //
