
#include "minimizerInterfaceC.h"

#include <map>
#include <mutex>


// Ownership, as the code stands:
//  * getSketcherMinimizer() returns a molecule the caller fills with setAtom/setBond. Until it is
//    run, the molecule owns its atoms and bonds.
//  * runGenerateCoordinates() hands it to a sketcherMinimizer. initialize() takes over the atoms
//    and bonds, splitIntoMolecules() puts the molecule itself into m_molecules, and
//    ~sketcherMinimizer -> clear() deletes all of them. The minimizer must therefore stay alive
//    while the caller reads results (getAtomX, isWedge, ...), and be deleted exactly once, after.
//  * freeSketcherMinimizer() does that. Every molecule handed out is tracked here, so freeing a
//    pointer twice, or one we never handed out, does nothing.
struct MinimizerEntry {
    sketcherMinimizer* minimizer;  // null until runGenerateCoordinates()
};
static std::mutex g_mutex;
static std::map<sketcherMinimizerMolecule*, MinimizerEntry> g_entries;

sketcherMinimizerMolecule* getSketcherMinimizer() {
    sketcherMinimizerMolecule* min_mol = new sketcherMinimizerMolecule();
    std::lock_guard<std::mutex> lock(g_mutex);
    g_entries[min_mol] = MinimizerEntry{nullptr};
    return min_mol;
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

void runGenerateCoordinates(sketcherMinimizerMolecule* min_mol) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_entries.find(min_mol);
        // Unknown molecule, or already run: a second minimizer would take over atoms the first
        // one still owns, so do nothing.
        if (it == g_entries.end() || it->second.minimizer != nullptr) {
            return;
        }
    }
    sketcherMinimizer* minimizer = new sketcherMinimizer();
    minimizer->initialize(min_mol);
    minimizer->runGenerateCoordinates();
    std::lock_guard<std::mutex> lock(g_mutex);
    g_entries[min_mol].minimizer = minimizer;
}

void freeSketcherMinimizer(sketcherMinimizerMolecule* min_mol) {
    MinimizerEntry entry{nullptr};
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_entries.find(min_mol);
        if (it == g_entries.end()) {
            return;  // not ours, or already freed
        }
        entry = it->second;
        g_entries.erase(it);
    }
    if (entry.minimizer != nullptr) {
        delete entry.minimizer;  // atoms, bonds, fragments and min_mol itself
    } else {
        // Never run: the molecule still owns its atoms and bonds, but
        // ~sketcherMinimizerMolecule only frees rings.
        for (auto* a : min_mol->getAtoms()) delete a;
        for (auto* b : min_mol->getBonds()) delete b;
        delete min_mol;
    }
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
