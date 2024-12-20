/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2024 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "pulseFixedValueFvPatchField.H"
#include "addToRunTimeSelectionTable.H"
#include "fieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
Foam::scalar Foam::pulseFixedValueFvPatchField<Type>::t() const
{
    return this->db().time().value();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::pulseFixedValueFvPatchField<Type>::
pulseFixedValueFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<Type>(p, iF),
    scalarData_(dict.lookup<scalar>("scalarData", unitAny)),
    data_(dict.lookup<Type>("data")),
    fieldData_("fieldData", iF.dimensions(), dict, p.size()),
    timeVsData_
    (
        Function1<Type>::New
        (
            "timeVsData",
            this->db().time().userUnits(),
            unitAny,
            dict
        )
    ),
    wordData_(dict.lookupOrDefault<word>("wordName", "wordDefault")),
    labelData_(-1),
    boolData_(false)
{


    fixedValueFvPatchField<Type>::evaluate();

    /*
    // Initialise with the value entry if evaluation is not possible
    fvPatchField<Type>::operator=
    (
        Field<Type>("value", iF.dimensions(), dict, p.size())
    );
    */
}


template<class Type>
Foam::pulseFixedValueFvPatchField<Type>::
pulseFixedValueFvPatchField
(
    const pulseFixedValueFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fieldMapper& mapper
)
:
    fixedValueFvPatchField<Type>(ptf, p, iF, mapper),
    scalarData_(ptf.scalarData_),
    data_(ptf.data_),
    fieldData_(mapper(ptf.fieldData_)),
    timeVsData_(ptf.timeVsData_, false),
    wordData_(ptf.wordData_),
    labelData_(-1),
    boolData_(ptf.boolData_)
{}


template<class Type>
Foam::pulseFixedValueFvPatchField<Type>::
pulseFixedValueFvPatchField
(
    const pulseFixedValueFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedValueFvPatchField<Type>(ptf, iF),
    scalarData_(ptf.scalarData_),
    data_(ptf.data_),
    fieldData_(ptf.fieldData_),
    timeVsData_(ptf.timeVsData_, false),
    wordData_(ptf.wordData_),
    labelData_(-1),
    boolData_(ptf.boolData_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::pulseFixedValueFvPatchField<Type>::map
(
    const fvPatchField<Type>& ptf,
    const fieldMapper& mapper
)
{
    fixedValueFvPatchField<Type>::map(ptf, mapper);

    const pulseFixedValueFvPatchField<Type>& tiptf =
        refCast<const pulseFixedValueFvPatchField<Type>>(ptf);

    mapper(fieldData_, tiptf.fieldData_);
}


template<class Type>
void Foam::pulseFixedValueFvPatchField<Type>::reset
(
    const fvPatchField<Type>& ptf
)
{
    fixedValueFvPatchField<Type>::reset(ptf);

    const pulseFixedValueFvPatchField<Type>& tiptf =
        refCast<const pulseFixedValueFvPatchField<Type>>(ptf);

    fieldData_.reset(tiptf.fieldData_);
}


template<class Type>
void Foam::pulseFixedValueFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    fixedValueFvPatchField<Type>::operator==
    (
        data_
      + fieldData_
      + scalarData_*timeVsData_->value(t())
    );


    fixedValueFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void Foam::pulseFixedValueFvPatchField<Type>::write
(
    Ostream& os
) const
{
    fvPatchField<Type>::write(os);
    writeEntry(os, "scalarData", scalarData_);
    writeEntry(os, "data", data_);
    writeEntry(os, "fieldData", fieldData_);
    writeEntry(os, this->db().time().userUnits(), unitAny, timeVsData_());
    writeEntry(os, "wordData", wordData_);
    writeEntry(os, "value", *this);
}



// ************************************************************************* //
