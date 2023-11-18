#include "MinimalUserInitialization.hh"

#include "G4Neutron.hh"
#include "G4NistManager.hh"
#include "G4PhysicsListHelper.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronInelasticProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4HadronFissionProcess.hh"

#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include "G4ThreeVector.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


#include "G4ParticleHPCaptureData.hh"
#include "G4ParticleHPElasticData.hh"
#include "G4ParticleHPFissionData.hh"
#include "G4ParticleHPInelasticData.hh"

#include "G4ParticleHPElastic.hh"
#include "G4ParticleHPInelastic.hh"
#include "G4ParticleHPFission.hh"
#include "G4ParticleHPCapture.hh"
//#include "G4ParticleHPBGGNucleonInelasticXS.hh"
//#include "G4ParticleHPJENDLHEInelasticData.hh"
//#include "G4NeutronBuilder.hh"
//#include "G4NeutronPHPBuilder.hh"
//#include "G4NeutronLENDBuilder.hh"


NeutronPhysicsList::NeutronPhysicsList() : G4VModularPhysicsList() {;}


void NeutronPhysicsList::ConstructHadronics() {
    
    G4PhysicsListHelper *ph = G4PhysicsListHelper::GetPhysicsListHelper();

    G4ParticleDefinition *theNeutron = G4Neutron::Definition();
    
    ph->RegisterProcess(new G4HadronElasticProcess(), theNeutron);
    ph->RegisterProcess(new G4HadronInelasticProcess("NeutronInelastic", theNeutron), theNeutron);
    ph->RegisterProcess(new G4HadronCaptureProcess(), theNeutron);
    ph->RegisterProcess(new G4HadronFissionProcess(), theNeutron);

    G4ProcessVector *procs = theNeutron->GetProcessManager()->GetProcessList();
    /*
    for (size_t i = 0; i < procs->size(); ++i) {
        G4String procName = (*procs)[i]->GetProcessName();
        G4String procType = G4VProcess::GetProcessTypeName((*procs)[i]->GetProcessType());

        std::cout << "Process " << i << " : " << " name = " << procName << ", type = " << procType << std::endl;

        (*procs)[i]->DumpInfo();
        (*procs)[i]->ProcessDescription(std::cout);
    }//*/

    static_cast<G4HadronicProcess*>((*procs)[1])->AddDataSet(new G4ParticleHPElasticData);
    static_cast<G4HadronicProcess*>((*procs)[2])->AddDataSet(new G4ParticleHPInelasticData);
    static_cast<G4HadronicProcess*>((*procs)[3])->AddDataSet(new G4ParticleHPCaptureData);
    static_cast<G4HadronicProcess*>((*procs)[4])->AddDataSet(new G4ParticleHPFissionData);

    static_cast<G4HadronicProcess*>((*procs)[1])->RegisterMe(new G4ParticleHPElastic);
    static_cast<G4HadronicProcess*>((*procs)[2])->RegisterMe(new G4ParticleHPInelastic);
    static_cast<G4HadronicProcess*>((*procs)[3])->RegisterMe(new G4ParticleHPCapture);
    static_cast<G4HadronicProcess*>((*procs)[4])->RegisterMe(new G4ParticleHPFission);

    //G4cout << "NeutronPhysicsList::ConstructHadronics()" << G4endl;
}




void NeutronPhysicsList::ConstructProcess() {
    AddTransportation();
    ConstructHadronics();
}


void NeutronPhysicsList::ConstructParticle() {
    G4ParticleDefinition *theNeutron = G4Neutron::NeutronDefinition();
    G4ParticleTable::GetParticleTable()->SetGenericIon(theNeutron);
}


MinimalDetector::MinimalDetector(G4String mat) : G4VUserDetectorConstruction(), material(mat) {;}

G4VPhysicalVolume* MinimalDetector::Construct() {
    G4Material *galactic = G4NistManager::Instance()->FindOrBuildMaterial(material);
    G4double l = 10.*m;
    G4Box *worldBox = new G4Box("WorldBox", l,l,l);
    return new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), new G4LogicalVolume(worldBox, galactic, "World"), "World", nullptr, false, 0, false);
}



MinimalInitialization::MinimalInitialization() : G4VUserActionInitialization() {;}

void MinimalInitialization::Build() const {
    SetUserAction(new MinimalPrimaryGenerator());
}


MinimalPrimaryGenerator::MinimalPrimaryGenerator(G4double energy) : G4VUserPrimaryGeneratorAction(), theEnergy(energy) {
    particleGun = new G4ParticleGun(1);
    particleGun->SetParticleDefinition(G4Neutron::Definition());
    //particleGun->SetParticleEnergy();
    particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
}


MinimalPrimaryGenerator::~MinimalPrimaryGenerator() {
    delete particleGun;
}



void MinimalPrimaryGenerator::GeneratePrimaries(G4Event *anEvent) {
    particleGun->SetParticleEnergy(theEnergy);
    particleGun->GeneratePrimaryVertex(anEvent);
}


