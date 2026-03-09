#!/bin/bash

################################################################################
CURRENTFOLDER=$PWD
echo "Work folder(indilib setup): $CURRENTFOLDER"
################################################################################

source BashUsefulFunctions/BashUsefulFunctions.incl.sh
ReadSettings $CURRENTFOLDER/settings.cfg [settings]
mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER
FunctionEchoOff
GetProgramName
FunctionEchoOn
GetNumberOfCPUs
RunAsRoot
REPO_VERSION=""
GetSystemTime
startTime=$secondscount

UpdateUsefulFunctions()
{
    cd $CURRENTFOLDER
    if [[ -d BashUsefulFunctions  ]];
    then
	cd BashUsefulFunctions
	git pull origin main
    else
	git clone https://github.com/radmilofelix/BashUsefulFunctions
    fi
}

InstallPrerequisites()
{
    InstallPrerequisites_list
#    InstallNvidia-cuda-toolkit
}

InstallPrerequisites_list()
{
    DisplayMessageInFrame "Upgrade system" 64 2 time
    apt-get update -y
    apt-get full-upgrade -y
    DisplayElapsedTime $startTime
    DisplayMessageInFrame "Install prerequisites" 64 2 time
#    InstallList settings.cfg [prerequisitesnouspiro]
    InstallList settings.cfg [prerequisites]
    DisplayElapsedTime $startTime
}

InstallNvidia-cuda-toolkit()
{
    cd $CURRENTFOLDER
    DisplayMessageInFrame "Install cuda-toolkit" 64 2 time
    mkdir $CURRENTFOLDER/$BUILDSOURCESFOLDER/cudaPackage
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/cudaPackage
    wget https://developer.download.nvidia.com/compute/cuda/repos/debian13/x86_64/cuda-keyring_1.1-1_all.deb
    sudo dpkg -i cuda-keyring_1.1-1_all.deb
    sudo apt-get update
    sudo apt-get -y install cuda-toolkit-13-1
    DisplayElapsedTime $startTime
}

GetINDIversion()
{
    DisplayMessageInFrame "Get INDI version code" 64 2 time
    # Capture the version line, then extract the version
    INDI_VERSION="$(indiserver --version 2>&1 | grep -oP '(?<=INDI Library: )\d+(\.\d+)+')"
    # Get the version code from settings.cfg
    if [ -n "$INDI_VERSION" ];
    then
	echo "The installed INDI library version is: $INDI_VERSION"
	REPO_VERSION=$(grep "$INDI_VERSION" $CURRENTFOLDER/settings.cfg | cut -d'=' -f2)
	if [ -z "$REPO_VERSION" ];
	then
		echo ""
		echo "Could not find the INDI library version code, abandoning script."
		echo "Check https://github.com/indilib/indi click on master/Tags"
		echo "and find the code value for indilib version $INDI_VERSION"
		echo "Add it to settings.cfg in [indiversion] section"
		echo ""
		GetSystemTime
		echo $PROGRAM_NAME", "$mtimeymdThms
		exit
	else
		echo "The installed INDI library version code value is: $REPO_VERSION"
	fi
    else
	    echo "No INDI installation detected!"
    fi
}

GitSetRepoCmd()
{
    git fetch origin
    git checkout $1
#    git switch -d --discard-changes $1
}

SetINDIcoreVersion() # alreadyInstalled(installed/-)
{
    if [[ $1 == "installed" ]];
    then
	GetINDIversion
	if [ -n "$REPO_VERSION" ];
	then
		echo "Building INDI version $REPO_VERSION."
		cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/indi
		GitSetRepoCmd "$REPO_VERSION"
	else
		echo "There is no INDI/KStars on the system, abandoning script!"
		GetSystemTime
		echo $PROGRAM_NAME", "$mtimeymdThms
		exit 1
	fi
    else
	    SetRepoVersion FORCEINDILIBVERSION indiversion INDIcore indi
    fi
}

SetINDI3rdPartyVersion() # alreadyInstalled(installed/-)
{
    if [[ $1 == "installed" ]];
    then
	GetINDIversion
	if [ -n "$REPO_VERSION" ];
	then
		echo "Building INDI version $REPO_VERSION."
		cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/indi-3rdparty
		GitSetRepoCmd "$REPO_VERSION"
	else
		echo "There is no INDI/KStars on the system, abandoning script!"
		GetSystemTime
		echo $PROGRAM_NAME", "$mtimeymdThms
		exit 1
	fi
    else
	    SetRepoVersion FORCEINDILIBVERSION indiversion INDIcore indi-3rdparty
    fi
}

SetRepoVersion() # FORCEDVARIABLE versionNameInSettings repoName gitName
{
    if [ -n "$1" ];
    then
	forcedVersion=$1
        echo "Forced $3 version defined!"
        ReadSettings $CURRENTFOLDER/settings.cfg [$2]
        FORCED_VERSION_VALUE="${!forcedVersion}"
        echo "Forced $3 version: $FORCED_VERSION_VALUE"
        REPO_VERSION="${!FORCED_VERSION_VALUE}"
        echo "REPO_VERSION: $REPO_VERSION"
        cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$4
        GitSetRepoCmd "$REPO_VERSION"
    else
        echo "Forced $3 version is not defined!"
        echo "Building the master version in the repositories."
    fi
}

BuildIndiCore()
{
    # https://github.com/indilib/indi
    DisplayMessageInFrame "Build INDI Core" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDIBASEBUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    git clone https://github.com/indilib/indi.git    
    SetINDIcoreVersion $1
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDIBASEBUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE $CURRENTFOLDER/$BUILDSOURCESFOLDER/indi
    make -j $NCPU
    make install
    DisplayElapsedTime $startTime
}

BuildINDI3rdParty()
{
    # https://github.com/indilib/indi-3rdparty
    DisplayMessageInFrame "Build INDI 3rd party drivers" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDI3RDPARTYBUILDFOLDER
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDI3RDPARTYLIBSBUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    git clone https://github.com/indilib/indi-3rdparty
    SetINDI3rdPartyVersion $1

    DisplayMessageInFrame "Build libraries" 64 2 time
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDI3RDPARTYLIBSBUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE -DBUILD_LIBS=1 $CURRENTFOLDER/$BUILDSOURCESFOLDER/indi-3rdparty
    make -j $NCPU
    make install

    DisplayMessageInFrame "Build drivers" 64 2 time
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$INDI3RDPARTYBUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE $CURRENTFOLDER/$BUILDSOURCESFOLDER/indi-3rdparty
    make -j $NCPU
    make install
    cd $CURRENTFOLDER
    DisplayElapsedTime $startTime
}

BuildStellarsolver()
{
    # https://github.com/rlancaste/stellarsolver
    DisplayMessageInFrame "Build StellarSolver" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$STELLARSOLVERBUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    git clone https://github.com/rlancaste/stellarsolver.git
    SetRepoVersion FORCESTELLARVERSION stellarsolverversion StellarSolver stellarsolver
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$STELLARSOLVERBUILDFOLDER
    if [ $BUILDSTELLARTESTER == "true" ];
    then
	echo "Building with StellarSolverTester"
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE -DBUILD_TESTER=ON $CURRENTFOLDER/$BUILDSOURCESFOLDER/stellarsolver
    else
	echo "Building without StellarSolverTester"
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE $CURRENTFOLDER/$BUILDSOURCESFOLDER/stellarsolver
    fi
    make -j $NCPU
    make install
    DisplayElapsedTime $startTime
}

BuildKstars()
{
    # https://invent.kde.org/education/kstars
    DisplayMessageInFrame "Build KStars" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$KSTARSBUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    #git clone https://anongit.kde.org/kstars.git
    git clone https://invent.kde.org/education/kstars.git
    SetRepoVersion FORCEKSTARSVERSION kstarsversion KStars kstars
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$KSTARSBUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE $CURRENTFOLDER/$BUILDSOURCESFOLDER/kstars
    #cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE -DBUILD_WITH_QT6=ON $CURRENTFOLDER/$BUILDSOURCESFOLDER/kstars
    make -j $NCPU
    make install
    DisplayElapsedTime $startTime
}

BuildPhd2()
{
    DisplayMessageInFrame "Build Phd2" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$PHD2BUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    git clone https://github.com/OpenPHDGuiding/phd2.git
    SetRepoVersion FORCEPHD2VERSION phd2version Phd2 phd2
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$PHD2BUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE $CURRENTFOLDER/$BUILDSOURCESFOLDER/phd2
    make -j $NCPU
    make install
    DisplayElapsedTime $startTime
}

BuildLibXisf()
{
    DisplayMessageInFrame "Build LibXISF" 64 2 time
    mkdir -p $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$LIBXISFBUILDFOLDER
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER
    git clone https://gitea.nouspiro.space/nou/libXISF.git
    SetRepoVersion FORCELIBXISFVERSION libxisfversion LibXISF libXISF
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER/$LIBXISFBUILDFOLDER
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=$BUILDTYPE -DUSE_BUNDLED_ZLIB=OFF $CURRENTFOLDER/$BUILDSOURCESFOLDER/libXISF
    make -j $NCPU
    make install
    DisplayElapsedTime $startTime
}

Copy-USB-ResetScript()
{
    cd $CURRENTFOLDER
    cp $USBRESETSCRIPTFOLDER/usb-reset.sh /bin
    chmod 755 /bin/usb-reset.sh
    echo "usb-reset.sh installed in the /bin folder."
}

Copy-UDEV-Rules()
{
    cd $CURRENTFOLDER
#    cp $UDEVFOLDER/99-usbto232devices.rules /etc/udev/rules.d
    cp $UDEVFOLDER/99-usbto232devices.rules /lib/udev/rules.d
    echo "UDEV rules for INDI serial drivers installed in /lib/udev/rules.d"
}


BuildAll()
{
    DisplayMessageInFrame "All build start" 64 2 time
    InstallPrerequisites
    if [ $BUILDLIBXISF == "true" ];
    then
        BuildLibXisf
    fi
    BuildIndiCore
    BuildINDI3rdParty
    BuildStellarsolver
    BuildKstars
    BuildMyDrivers
    if [ $BUILDPHD2 == "true" ];
    then
        BuildPhd2
    fi
    DisplayMessageInFrame "Copy scripts" 64 2 time
    Copy-USB-ResetScript
    Copy-UDEV-Rules
    if [ $BUILDSTELLARTESTER == "true" ];
    then
        echo
        echo "StellarSolver was built with StellarSolverTester."
        echo "To start the tester, type \"StellarSolverTester\" in a terminal"
        echo
    fi
    DisplayMessageInFrame "All build end" 64 2 time
    echo "Build finished,"
    DisplayElapsedTime $startTime
    usb-reset.sh all.
}

InstallAll()
{
    InstallPrerequisites
    InstallKStars
    BuildAfterInstall
    echo "Installation finished,"
    DisplayElapsedTime $startTime
}

BuildAfterInstall()
{
    DisplayMessageInFrame "All install start" 64 2 time
    BuildIndiCore installed
    BuildINDI3rdParty installed
    BuildMyDrivers
    DisplayMessageInFrame "Copy scripts" 64 2 time
    Copy-USB-ResetScript
    Copy-UDEV-Rules
    usb-reset.sh all
    DisplayMessageInFrame "All install end" 64 2 time
}

BuildBasic()
{
    InstallPrerequisites
    BuildIndiCore
}

CleanAll()
{
    cd $CURRENTFOLDER
    #rm -rf $BUILDSOURCESFOLDER
}

BuildMyDrivers()
{
    DisplayMessageInFrame "Build my drivers" 64 2 time
    cd $CURRENTFOLDER/$MYDRIVERSFOLDER
    chmod 775 buildall.sh
    chmod 775 buildflats.sh
    chmod 775 buildfocuser.sh
    chmod 775 buildheater.sh
    chmod 775 buildroof.sh
    chmod 775 buildweather.sh
    ./buildall.sh build
    DisplayElapsedTime $startTime
}

RemoveIndiBase()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $INDIBASEBUILDFOLDER/install_manifest.txt ] && echo "Removing INDI base"; cat $INDIBASEBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemoveIndi3rdParty()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $INDI3RDPARTYLIBSBUILDFOLDER/install_manifest.txt ] && echo "Removing INDI 3rdparty libs"; cat $INDI3RDPARTYLIBSBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    [ -f $INDI3RDPARTYBUILDFOLDER/install_manifest.txt ] && echo "Removing INDI 3rdparty drivers"; cat $INDI3RDPARTYBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemoveStellarSolver()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $STELLARSOLVERBUILDFOLDER/install_manifest.txt ] && echo "Removing StellarSolver"; cat $STELLARSOLVERBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemoveKstars()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $KSTARSBUILDFOLDER/install_manifest.txt ] && echo "Removing KStars"; cat $KSTARSBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemovePhd2()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $PHD2BUILDFOLDER/install_manifest.txt ] && echo "Removing KStars"; cat $PHD2BUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemoveLibxisf()
{
    cd $CURRENTFOLDER/$BUILDSOURCESFOLDER/$BUILDFOLDER
    [ -f $LIBXISFBUILDFOLDER/install_manifest.txt ] && echo "Removing Libxisf"; cat $LIBXISFBUILDFOLDER/install_manifest.txt | sudo xargs rm -f
    DisplayElapsedTime $startTime
}

RemoveAll()
{
    DisplayMessageInFrame "Remove all" 64 2 time
    RemoveIndiBase
    RemoveIndi3rdParty
    RemoveStellarSolver
    RemoveKstars
    RemovePhd2
    RemoveLibxisf
    echo "All built software was removed."
    DisplayElapsedTime $startTime
}

InstallKStars()
{
    DisplayMessageInFrame "Install INDI & KStars" 64 2 time
    GetDistroName
    if [[ "$DISTRO_NAME" != "Ubuntu" ]]
    then
        echo "This distro name is not Ubuntu but $DISTRO_NAME"
        echo "Kstars installation from PPA only works on Ubuntu."
        echo "Abandoning script..."
        GetSystemTime
        echo $PROGRAM_NAME", "$mtimeymdThms
	exit
fi

    sudo apt-add-repository ppa:mutlaqja/ppa -y
    sudo apt-get update
    sudo apt-get install -y indi-full gsc kstars-bleeding

    sudo cp $CURRENTFOLDER/$UDEVFOLDER/99-usbto232devices.rules /lib/udev/rules.d
    sudo udevadm control --reload
    sudo udevadm trigger
    DisplayElapsedTime $startTime

    #If you receive an error about some package trying to overwrite another, try this:
    #sudo apt -o Dpkg::Options::="--force-overwrite" -f install

    #Nightly unstable builds for INDI and KStars are available. Do NOT use this in production. It is only available for testing purposes.
    #sudo apt-add-repository ppa:mutlaqja/indinightly
    #sudo apt-get update
    #sudo apt-get install indi-full kstars-bleeding gsc

    #If you would like to stop using Nightly PPA builds and instead return to using stable PPA builds then run the following commands:
    #sudo apt-get install ppa-purge
    #sudo ppa-purge ppa:mutlaqja/indinightly 
}

help()
{
    echo
    echo "INDI drivers build script help:"
    echo
    echo "Usage: `basename "$0"` < UpdateUsefulFunctions | BuildAll | InstallAll | BuildBasic | InstallPrerequisites | BuildIndiCore | compileIndibase"
    echo "GetINDIversion | GetDistroName | BuildINDI3rdParty | BuildStellarsolver | BuildKstars | CleanAll | RemoveAll | BuildMyDrivers | help >"
    echo
    echo "You can build everything (BuildAll) both on Ubuntu or Debian systems or flavours."
    echo
    echo "On Debian systems you can not install KStars with this script, so do not use InstallAll option."
    echo "On Debian systems the default KStars installation (apt install KStars) will install a quite old version."
    echo
    echo "Kstars can be installed by this script only on Ubuntu systems (InstallAll option)."
    echo
    echo "You can choose the versions of the programs to be installed by editing the FORCE... variables in settings.cfg"
    echo
    echo
}

test()
{
	echo Test
#	BuildIndiCore
#	BuildIndiCore installed
}


if [ -z $1 ]
then
    help
else
    $1
fi
