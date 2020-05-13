
#include "sketcherMinimizer.h"

class sketcherMinimizer;
class sketcherMinimizerAtom;
class sketcherMinimizerBond;

extern "C" EXPORT_COORDGEN sketcherMinimizerMolecule* getSketcherMinimizer();

extern "C" EXPORT_COORDGEN sketcherMinimizerAtom* setAtom(
    sketcherMinimizerMolecule* min_mol,
    int number
);

extern "C" EXPORT_COORDGEN sketcherMinimizerBond* setBond(
    sketcherMinimizerMolecule* min_mol,
    sketcherMinimizerAtom* u,
    sketcherMinimizerAtom* v,
    int order
);

extern "C" EXPORT_COORDGEN void assignBondsAndNeighbors(
    sketcherMinimizerMolecule* min_mol
);

extern "C" EXPORT_COORDGEN void setStereoCenter(
    sketcherMinimizerAtom* center,
    sketcherMinimizerAtom* lookingFrom,
    sketcherMinimizerAtom* atom1,
    sketcherMinimizerAtom* atom2,
    bool isClockwise
);

extern "C" EXPORT_COORDGEN void setStereoBond(
    sketcherMinimizerBond* bond,
    sketcherMinimizerAtom* atom1,
    sketcherMinimizerAtom* atom2,
    bool isCis
);

extern "C" EXPORT_COORDGEN void runGenerateCoordinates(
    sketcherMinimizerMolecule* min_mol
);

extern "C" EXPORT_COORDGEN float getAtomX(sketcherMinimizerAtom* atom);
extern "C" EXPORT_COORDGEN float getAtomY(sketcherMinimizerAtom* atom);

extern "C" EXPORT_COORDGEN bool hasStereochemistryDisplay(
    sketcherMinimizerBond* bond
);
extern "C" EXPORT_COORDGEN bool isWedge(sketcherMinimizerBond* bond);
extern "C" EXPORT_COORDGEN bool isReversed(sketcherMinimizerBond* bond);
