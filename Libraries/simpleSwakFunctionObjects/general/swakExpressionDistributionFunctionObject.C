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
    2008-2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionDistributionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(swakExpressionDistributionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        swakExpressionDistributionFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakExpressionDistributionFunctionObject::swakExpressionDistributionFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    distributionFunctionObject(name,t,dict),
    expression_(
        dict.lookup("expression"),
        dict
    ),
    weightExpression_(
        dict.lookup("weight"),
        dict
    ),
    maskExpression_(
        dict.lookup("mask"),
        dict
    ),
    driver_(
        CommonValueExpressionDriver::New(
            dict,
            refCast<const fvMesh>(obr_)
        )
    )
{
    driver_->createWriterAndRead(name+"_"+type());
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word swakExpressionDistributionFunctionObject::dirName()
{
    return typeName;
}

word swakExpressionDistributionFunctionObject::baseName()
{
    return "expression";
}

void swakExpressionDistributionFunctionObject::getDistribution()
{
    word rType=driver_->CommonValueExpressionDriver::getResultType();

    if(rType==pTraits<scalar>::typeName) {
        getDistributionInternal(
            distScalar_,
            weightValuesScalar_
        );
    } else if(rType==pTraits<vector>::typeName) {
        getDistributionInternal(
            distVector_,
            weightValuesVector_
        );
    } else if(rType==pTraits<vector>::typeName) {
        getDistributionInternal(
            distTensor_,
            weightValuesTensor_
        );
    } else if(rType==pTraits<vector>::typeName) {
        getDistributionInternal(
            distSymmTensor_,
            weightValuesSymmTensor_
        );
    } else if(rType==pTraits<vector>::typeName) {
        getDistributionInternal(
            distSphericalTensor_,
            weightValuesSphericalTensor_
        );
    } else {
        WarningIn("swakExpressionDistributionFunctionObject::getDistribution()")
            << "Don't know how to handle type " << rType
                << endl;
    }
}

void swakExpressionDistributionFunctionObject::writeSimple()
{
    if(verbose()) {
        Info << "DistributionExpression " << name() << " : ";
    }

    driver_->clearVariables();

    driver_->parse(weightExpression_);
    if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<scalar>::typeName

    ) {
        weightValuesScalar_.reset(
            driver_->getResult<scalar>().ptr()
        );
    } else if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<vector>::typeName

    ) {
        weightValuesVector_.reset(
            driver_->getResult<vector>().ptr()
        );
    } else if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<tensor>::typeName

    ) {
        weightValuesTensor_.reset(
            driver_->getResult<tensor>().ptr()
        );
    } else if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<symmTensor>::typeName

    ) {
        weightValuesSymmTensor_.reset(
            driver_->getResult<symmTensor>().ptr()
        );
    } else if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<sphericalTensor>::typeName

    ) {
        weightValuesSphericalTensor_.reset(
            driver_->getResult<sphericalTensor>().ptr()
        );
    } else {
        FatalErrorIn("swakExpressionDistributionFunctionObject::writeSimple()")
            << "Don't know how to handle weight expressions of type "
                << driver_->CommonValueExpressionDriver::getResultType()
                << endl
                << exit(FatalError);

    }

    driver_->parse(maskExpression_);
    if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<bool>::typeName
    ) {
        maskValues_.reset(
            driver_->getResult<bool>().ptr()
        );
    } else if(
        driver_->CommonValueExpressionDriver::getResultType()
        ==
        pTraits<scalar>::typeName
    ) {
        scalarField rawMask(driver_->getResult<scalar>());
        maskValues_.reset(
            new Field<bool>(rawMask.size(),false)
        );
        forAll(rawMask,i) {
            if(rawMask[i]>SMALL) {
                maskValues_()[i]=true;
            }
        }
    } else {
        FatalErrorIn("swakExpressionDistributionFunctionObject::writeSimple()")
            << "Don't know how to handle logical expressions of type "
                << driver_->CommonValueExpressionDriver::getResultType()
                << endl
                << exit(FatalError);

    }

    driver_->parse(expression_);

    distributionFunctionObject::writeSimple();

    if(verbose()) {
        Info << endl;
    }

    // make sure that the stored Variables are consistently written
    driver_->tryWrite();
}

} // namespace Foam

// ************************************************************************* //
