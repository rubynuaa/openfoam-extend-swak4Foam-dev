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

Class
    patchFieldFlowFunctionObject

Description
    Averages the flow of fields on the patches

SourceFiles
    patchFieldFlowFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef patchFieldFlowFunctionObject_H
#define patchFieldFlowFunctionObject_H

#include "patchFieldFunctionObject.H"
#include "SolverInfo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchFieldFlowFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchFieldFlowFunctionObject
:
    public patchFieldFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchFieldFlowFunctionObject(const patchFieldFlowFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const patchFieldFlowFunctionObject&);

    //- integrate single field on all patches
    template <class T>
    Field<T> fieldFlow(const word& fieldName,T preset) const;
    
    SolverInfo solver_;

protected:

    word dirName();

    virtual scalarField process(const word& fieldName,scalar preset);
    virtual Field<vector> process(const word& fieldName,vector preset);
    virtual Field<sphericalTensor> process(const word& fieldName,sphericalTensor preset);
    virtual Field<symmTensor> process(const word& fieldName,symmTensor preset);
    virtual Field<tensor> process(const word& fieldName,tensor preset);

public:

    //- Runtime type information
    TypeName("patchFieldFlow");


    // Constructors

    //- Construct from components
    patchFieldFlowFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "patchFieldFlowFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
