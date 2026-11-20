
#include "minimizerInterfaceC.h"

// Ownership: getSketcherMinimizer() returns a molecule the caller fills with setAtom/setBond.
// runGenerateCoordinates() hands it to a new sketcherMinimizer, which then owns the atoms, bonds
// and the molecule itself, and returns it. The caller reads the results (getAtomX, isWedge, ...)
// and then calls freeSketcherMinimizer(), which frees all of it.
sketcherMinimizerMolecule* getSketcherMinimizer() {
    return new sketcherMinimizerMolecule();
}

sketcherMinimizerAtom* setAtom(
    sketcherMinimizerMolecule* min_mol,
    int number
) {
    sketcherMinimizerAtom* atom = min_mol->addNewAtom();
    atom->setAtomicNumber(number);
    return atom;
}

sketcherMinimizerBond* setBond(
    sketcherMinimizerMolecule* min_mol,
    sketcherMinimizerAtom* u,
    sketcherMinimizerAtom* v,
    int order
) {
    sketcherMinimizerBond* bond = min_mol->addNewBond(u, v);
    bond->setBondOrder(order);
    return bond;
}

void assignBondsAndNeighbors(
    sketcherMinimizerMolecule* min_mol
) {
    auto atoms = min_mol->getAtoms();
    auto bonds = min_mol->getBonds();
    min_mol->assignBondsAndNeighbors(atoms, bonds);
}

void setStereoCenter(
    sketcherMinimizerAtom* center,
    sketcherMinimizerAtom* lookingFrom,
    sketcherMinimizerAtom* atom1,
    sketcherMinimizerAtom* atom2,
    bool isClockwise
) {
    struct sketcherMinimizerAtomChiralityInfo ci;
    ci.lookingFrom = lookingFrom;
    ci.atom1 = atom1;
    ci.atom2 = atom2;
    ci.direction = isClockwise ? sketcherMinimizerAtomChiralityInfo::clockwise : sketcherMinimizerAtomChiralityInfo::counterClockwise;
    center->setStereoChemistry(ci);
    center->setAbsoluteStereoFromChiralityInfo();
}

void setStereoBond(
    sketcherMinimizerBond* bond,
    sketcherMinimizerAtom* atom1,
    sketcherMinimizerAtom* atom2,
    bool isCis
) {
    struct sketcherMinimizerBondStereoInfo si;
    si.atom1 = atom1;
    si.atom2 = atom2;
    si.stereo = isCis ? sketcherMinimizerBondStereoInfo::cis : sketcherMinimizerBondStereoInfo::trans;
    bond->setStereoChemistry(si);
    bond->setAbsoluteStereoFromStereoInfo();
}

sketcherMinimizer* runGenerateCoordinates(sketcherMinimizerMolecule* min_mol) {
    sketcherMinimizer* minimizer = new sketcherMinimizer();
    minimizer->initialize(min_mol);
    minimizer->runGenerateCoordinates();
    return minimizer;
}

void freeSketcherMinimizer(sketcherMinimizer* minimizer) {
    delete minimizer;
}

float getAtomX(sketcherMinimizerAtom* atom) {
    return atom->getCoordinates().x();
}

float getAtomY(sketcherMinimizerAtom* atom) {
    return atom->getCoordinates().y();
}

bool hasStereochemistryDisplay(sketcherMinimizerBond* bond) {
    return bond->hasStereochemistryDisplay;
}

bool isWedge(sketcherMinimizerBond* bond) {
    return bond->isWedge;
}

bool isReversed(sketcherMinimizerBond* bond) {
    return bond->isReversed;
}
