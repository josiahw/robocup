#include "WalkParameterWidget.h"

// Qt Includes
#include <QMdiArea>
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QMdiSubWindow>
//#include <QStringlist>
#include <QPixmap>
#include <QPushButton>
#include <QSignalMapper>
#include <QPainter>
#include <QToolButton>
#include <QColorDialog>
#include <QTimer>

#include <typeinfo>
#include "GLDisplay.h"

#include "Infrastructure/Jobs/Jobs.h"
#include "NUviewIO/NUviewIO.h"
#include "Tools/Optimisation/Parameter.h"
#include "debug.h"

WalkParameterWidget::WalkParameterWidget(QMdiArea* parentMdiWidget, QWidget *parent): QWidget(parent)
{
    setObjectName(tr("Walk Parameter(s)"));
    setWindowTitle(tr("Walk Parameter(s)"));

    m_job_list = new JobList();
    m_walk_parameters.load("JuppWalkTest");
    
    createWidgets();
    createLayout();
    createConnections();
    this->setEnabled(true);
    disableWriting = false;
}

void WalkParameterWidget::createWidgets()
{    
    vector<Parameter>& params = m_walk_parameters.getParameters();
    vector<float>& maxspeeds = m_walk_parameters.getMaxSpeeds();
    
    // Shift Amplitude
    shiftAmplitudeLabel = new QLabel("Amplitude");
    shiftAmplitudeSlider = new QSlider(Qt::Horizontal);
    shiftAmplitudeSlider->setMinimum(1);
    shiftAmplitudeSlider->setMaximum(25);

    shiftAmplitudeSpinBox = new QSpinBox();
    shiftAmplitudeSpinBox->setMinimum(shiftAmplitudeSlider->minimum());
    shiftAmplitudeSpinBox->setMaximum(shiftAmplitudeSlider->maximum());
    
    // Shift Frequency
    shiftFrequencyLabel = new QLabel("Frequency");
    shiftFrequencySlider = new QSlider(Qt::Horizontal);
    shiftFrequencySlider->setMinimum(1);
    shiftFrequencySlider->setMaximum(30);
    
    shiftFrequencySpinBox = new QSpinBox();
    shiftFrequencySpinBox->setMinimum(shiftFrequencySlider->minimum());
    shiftFrequencySpinBox->setMaximum(shiftFrequencySlider->maximum());
    if (params.size() > 0)
    {
        shiftFrequencySlider->setValue(params[0].get()*10);
        shiftFrequencySpinBox->setValue(params[0].get()*10);
    }
    
    // Phase Offset
    phaseOffsetLabel = new QLabel("Phase");
    phaseOffsetSlider = new QSlider(Qt::Horizontal);
    phaseOffsetSlider->setMinimum(-100);
    phaseOffsetSlider->setMaximum(100);
    
    phaseOffsetSpinBox = new QSpinBox();
    phaseOffsetSpinBox->setMinimum(phaseOffsetSlider->minimum());
    phaseOffsetSpinBox->setMaximum(phaseOffsetSlider->maximum());
    
    // Phase Reset
    phaseResetLabel = new QLabel("Phase Reset");
    phaseResetSlider = new QSlider(Qt::Horizontal);
    phaseResetSlider->setMinimum(-100);
    phaseResetSlider->setMaximum(100);
    
    phaseResetSpinBox = new QSpinBox();
    phaseResetSpinBox->setMinimum(phaseResetSlider->minimum());
    phaseResetSpinBox->setMaximum(phaseResetSlider->maximum());
    
    // X Speed
    xSpeedLabel = new QLabel("x (mm/s)");
    xSpeedSlider = new QSlider(Qt::Horizontal);
    xSpeedSlider->setMinimum(0);
    xSpeedSlider->setMaximum(300);
    
    xSpeedSpinBox = new QSpinBox();
    xSpeedSpinBox->setMinimum(xSpeedSlider->minimum());
    xSpeedSpinBox->setMaximum(xSpeedSlider->maximum());
    
    // Y Speed
    ySpeedLabel = new QLabel("y (mm/s)");
    ySpeedSlider = new QSlider(Qt::Horizontal);
    ySpeedSlider->setMinimum(0);
    ySpeedSlider->setMaximum(300);
    
    ySpeedSpinBox = new QSpinBox();
    ySpeedSpinBox->setMinimum(ySpeedSlider->minimum());
    ySpeedSpinBox->setMaximum(ySpeedSlider->maximum());
    
    // Yaw Speed
    yawSpeedLabel = new QLabel("yaw (crad/s)");
    yawSpeedSlider = new QSlider(Qt::Horizontal);
    yawSpeedSlider->setMinimum(0);
    yawSpeedSlider->setMaximum(100);
    
    yawSpeedSpinBox = new QSpinBox();
    yawSpeedSpinBox->setMinimum(yawSpeedSlider->minimum());
    yawSpeedSpinBox->setMaximum(yawSpeedSlider->maximum());
    if (maxspeeds.size() > 2)
    {
        xSpeedSlider->setValue(maxspeeds[0]*10);
        xSpeedSpinBox->setValue(maxspeeds[0]*10);
        ySpeedSlider->setValue(maxspeeds[1]*10);
        ySpeedSpinBox->setValue(maxspeeds[1]*10);
        yawSpeedSlider->setValue(maxspeeds[2]*100);
        yawSpeedSpinBox->setValue(maxspeeds[2]*100);
    }
}

void WalkParameterWidget::createLayout()
{
    // Shift Amplitude 
    shiftAmplitudeLayout = new QHBoxLayout();
    shiftAmplitudeLayout->addWidget(shiftAmplitudeLabel);
    shiftAmplitudeLayout->addWidget(shiftAmplitudeSlider);
    shiftAmplitudeLayout->addWidget(shiftAmplitudeSpinBox);
    
    // Shift Frequency 
    shiftFrequencyLayout = new QHBoxLayout();
    shiftFrequencyLayout->addWidget(shiftFrequencyLabel);
    shiftFrequencyLayout->addWidget(shiftFrequencySlider);
    shiftFrequencyLayout->addWidget(shiftFrequencySpinBox);
    
    // Phase Offset 
    phaseOffsetLayout = new QHBoxLayout();
    phaseOffsetLayout->addWidget(phaseOffsetLabel);
    phaseOffsetLayout->addWidget(phaseOffsetSlider);
    phaseOffsetLayout->addWidget(phaseOffsetSpinBox);
    
    // Phase Reset 
    phaseResetLayout = new QHBoxLayout();
    phaseResetLayout->addWidget(phaseResetLabel);
    phaseResetLayout->addWidget(phaseResetSlider);
    phaseResetLayout->addWidget(phaseResetSpinBox);
    
    // X Speed 
    xSpeedLayout = new QHBoxLayout();
    xSpeedLayout->addWidget(xSpeedLabel);
    xSpeedLayout->addWidget(xSpeedSlider);
    xSpeedLayout->addWidget(xSpeedSpinBox);

    // Y Speed 
    ySpeedLayout = new QHBoxLayout();
    ySpeedLayout->addWidget(ySpeedLabel);
    ySpeedLayout->addWidget(ySpeedSlider);
    ySpeedLayout->addWidget(ySpeedSpinBox);
    
    // Y Speed 
    yawSpeedLayout = new QHBoxLayout();
    yawSpeedLayout->addWidget(yawSpeedLabel);
    yawSpeedLayout->addWidget(yawSpeedSlider);
    yawSpeedLayout->addWidget(yawSpeedSpinBox);
    
    // Setup overall layout
    overallLayout = new QVBoxLayout();
    overallLayout->addLayout(shiftAmplitudeLayout);
    overallLayout->addLayout(shiftFrequencyLayout);
    overallLayout->addLayout(phaseOffsetLayout);
    overallLayout->addLayout(phaseResetLayout);
    overallLayout->addLayout(xSpeedLayout);
    overallLayout->addLayout(ySpeedLayout);
    overallLayout->addLayout(yawSpeedLayout);
    setLayout(overallLayout);
}

void WalkParameterWidget::createConnections()
{
    // Setup the timer, and connect it to the walkParameter changed
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(walkParameterChanged()));
    m_timer->start(1000);
}

WalkParameterWidget::~WalkParameterWidget()
{
    delete m_timer;
    // Shift Amplitude
    delete shiftAmplitudeLabel;      
    delete shiftAmplitudeSlider;     
    delete shiftAmplitudeSpinBox;    
    delete shiftAmplitudeLayout;
    
    // Shift Frequency
    delete shiftFrequencyLabel;      
    delete shiftFrequencySlider;    
    delete shiftFrequencySpinBox;   
    delete shiftFrequencyLayout; 
    
    // Phase Offset
    delete phaseOffsetLabel;      
    delete phaseOffsetSlider;    
    delete phaseOffsetSpinBox;   
    delete phaseOffsetLayout; 
    
    // Phase Offset
    delete phaseResetLabel;      
    delete phaseResetSlider;    
    delete phaseResetSpinBox;   
    delete phaseResetLayout; 
    
    // X speed
    delete xSpeedLabel;           
    delete xSpeedSlider;          
    delete xSpeedSpinBox;    
    delete xSpeedLayout; 
    
    // Y speed
    delete ySpeedLabel;           
    delete ySpeedSlider;          
    delete ySpeedSpinBox;    
    delete ySpeedLayout; 
    
    // yaw speed
    delete yawSpeedLabel;           
    delete yawSpeedSlider;          
    delete yawSpeedSpinBox;    
    delete yawSpeedLayout; 
    
    delete overallLayout;
    delete m_job_list;
}

void WalkParameterWidget::walkParameterChanged()
{
    m_walk_parameters.load("JuppWalkTest");
    
    m_job_list->addMotionJob(new WalkParametersJob(m_walk_parameters));
    m_job_list->summaryTo(debug);

    //(*nuio) << m_job_list;
    
    m_job_list->clear();
}


