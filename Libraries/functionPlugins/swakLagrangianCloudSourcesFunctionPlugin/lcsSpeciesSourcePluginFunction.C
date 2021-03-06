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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "lcsSpeciesSourcePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "BasicReactingCloud.H"
#include "BasicReactingMultiphaseCloud.H"
#else
#include "basicReactingCloud.H"
#include "basicReactingMultiphaseCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(lcsSpeciesSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsSpeciesSourcePluginFunction , name, lcsSpeciesSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsSpeciesSourcePluginFunction::lcsSpeciesSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    LagrangianCloudSourcePluginFunction(
        parentDriver,
        name,
        "volScalarField",
        "speciesName primitive word"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<lcsSpeciesSourcePluginFunction::dimScalarField>
lcsSpeciesSourcePluginFunction::internalEvaluate(const label speciesIndex)
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(dimScalarField,constThermoReactingCloud,reactingCloud,Srho(speciesIndex));
    tryCall(dimScalarField,thermoReactingCloud,reactingCloud,Srho(speciesIndex));
    tryCall(dimScalarField,icoPoly8ThermoReactingCloud,reactingCloud,Srho(speciesIndex));
    tryCall(dimScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,Srho(speciesIndex));
    tryCall(dimScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,Srho(speciesIndex));
    tryCall(dimScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,Srho(speciesIndex));
#else
    tryCall(dimScalarField,basicReactingCloud,reactingCloud,Srho(speciesIndex));
    tryCall(dimScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,Srho(speciesIndex));
#endif

    return autoPtr<dimScalarField>();
}

label lcsSpeciesSourcePluginFunction::getIndex(wordList &speciesList)
{
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    getSpeciesIndex(constThermoReactingCloud,reactingCloud);
    getSpeciesIndex(thermoReactingCloud,reactingCloud);
    getSpeciesIndex(icoPoly8ThermoReactingCloud,reactingCloud);
    getSpeciesIndex(constThermoReactingMultiphaseCloud,reactingMultiphaseCloud);
    getSpeciesIndex(thermoReactingMultiphaseCloud,reactingMultiphaseCloud);
    getSpeciesIndex(icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud);
#else
    getSpeciesIndex(basicReactingCloud,reactingCloud);
    getSpeciesIndex(basicReactingMultiphaseCloud,reactingMultiphaseCloud);
#endif

    return -1;
}

void lcsSpeciesSourcePluginFunction::doEvaluation()
{
    wordList speciesList;
    label speciesIndex=getIndex(speciesList);

    if(speciesList.size()==0) {
        listAvailableClouds(Info);
        FatalErrorIn("lcsSpeciesSourcePluginFunction::doEvaluation()")
            << "No species list found (probably wrong cloud type)"
                << endl
                << "Supported cloud types (typeid): "                   \
                << supportedTypes() << endl                             \
                << exit(FatalError);
    }

    if(speciesIndex<0) {
        listAvailableClouds(Info);
        FatalErrorIn("lcsSpeciesSourcePluginFunction::doEvaluation()")
            << "Species " << speciesName_ << " not found in gas composition "
                << speciesList
                << endl
                << "Supported cloud types (typeid): "                   \
                << supportedTypes() << endl                             \
                << exit(FatalError);

    }

    autoPtr<dimScalarField> pSrho=internalEvaluate(speciesIndex);;

    noCloudFound(pSrho);

    const dimScalarField &Srho=pSrho();

    autoPtr<volScalarField> pSource(
        new volScalarField(
            IOobject(
                cloudName()+"SpeciesSource",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            Srho.dimensions(),
            "zeroGradient"
        )
    );

#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<scalarField&>(pSource->internalField().field())
#else
    pSource->internalField()
#endif
        =Srho.field();

    result().setObjectResult(pSource);
}

void lcsSpeciesSourcePluginFunction::setArgument(
    label index,
    const word &name
)
{
    if(index==0) {
        LagrangianCloudSourcePluginFunction::setArgument(
            index,
            name
        );
        return;
    }
    assert(index==1);
    speciesName_=name;
}



// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
