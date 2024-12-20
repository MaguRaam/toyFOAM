# boundaryCondition

Boundary conditions are implemented in `$FOAM_SRC/finiteVolume/fields/fvPatchFields`. To list them

```bash
foamToC -table fvPatchField
foamToC -table fvPatchScalarField
```

Run `foamNewBc` to get a ready made code
```bash
foamNewBC -help
foamNewBC -fixedValue -all pulseFixedValue
```

To build a library
```bash
cd pulseFixedValue
wmake

Now we can edit it to our taste