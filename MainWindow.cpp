//============================================================================
// Name        : MainScreen.cpp
// Author      : zhabakaba
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "MainWindow.h"

using namespace std;

extern Dispatcher theDispatcher;

static const char CH_CMD_DELIM = ' ';

bool isNumber(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

MainWindow* Load_Glade(std::string strFile)
{
	//Load the GtkBuilder file and instantiate its widgets:
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_file(strFile);
	}
	catch(const Glib::FileError& ex)
	{
		std::cerr << "FileError: " << ex.what() << std::endl;
		return nullptr;
	}
	catch(const Glib::MarkupError& ex)
	{
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return nullptr;
	}
	catch(const Gtk::BuilderError& ex)
	{
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return nullptr;
	}

	//Get the GtkBuilder-instantiated Dialog:
	MainWindow *pWndMain = nullptr;
	refBuilder->get_widget_derived("WindowMain", pWndMain);

	return pWndMain;
}

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
	: Gtk::Window(cobject),
	  m_refGlade(refGlade)
{
	// Global
	this->signal_hide().connect(sigc::mem_fun(*this, &MainWindow::on_window_hide));

	m_refGlade->get_widget("App_About", m_pButtonAbout);
	if (m_pButtonAbout)
		m_pButtonAbout->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_about));

	m_refGlade->get_widget("App_Exit", m_pButtonExit);
	if (m_pButtonExit)
		m_pButtonExit->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_quit));

	m_refGlade->get_widget("App_Arhive", m_pButtonArchive);
	if (m_pButtonArchive)
		m_pButtonArchive->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Archive));

	m_refGlade->get_widget("App_Log", m_pButtonLog);
	if (m_pButtonLog)
		m_pButtonLog->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Log));

	m_refGlade->get_widget("App_Error", m_pButtonError);
	if (m_pButtonError)
		m_pButtonError->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Error));

	m_refGlade->get_widget("Arm_Stack", m_pStack);
	m_refGlade->get_widget("System_Status", m_pLabelStatus);
	m_refGlade->get_widget("System_Status_Bk", m_pBoxStatus);

	// Init
	m_refGlade->get_widget("Init_Error", m_pInitLabelError);

	m_refGlade->get_widget("Init_Entry_Login", m_pInitEntryLogin);
	if (m_pInitEntryLogin)
		m_pInitEntryLogin->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::init_on_entry_changed));

	m_refGlade->get_widget("Init_Entry_Password", m_pInitEntryPassword);
	if (m_pInitEntryPassword)
		m_pInitEntryPassword->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::init_on_entry_changed));

	m_refGlade->get_widget("Init_Button_Enter", m_pInitButtonEnter);
	if (m_pInitButtonEnter)
		m_pInitButtonEnter->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::init_on_button_enter));

	m_pInitEntryPassword->set_visibility(false);

	m_pInitLabelError->hide();

	// Work
	m_refGlade->get_widget("Work_Status_1", m_pWorkLabelStatus[0]);
	m_refGlade->get_widget("Work_Status_2", m_pWorkLabelStatus[1]);

	m_refGlade->get_widget("Work_Label_Ready", m_pWorkLabelReady);
	m_refGlade->get_widget("Work_Label_Role", m_pWorkLabelRole);
	m_refGlade->get_widget("Work_Label_Configuration", m_pWorkLabelConfiguration);

	m_refGlade->get_widget("Work_Button_Close", m_pWorkButtonClose);
	if (m_pWorkButtonClose)
		m_pWorkButtonClose->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_on_close_button));

	m_refGlade->get_widget("Work_Setting", m_pWorkButtonSetting);
	if (m_pWorkButtonSetting)
		m_pWorkButtonSetting->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Settings));

	m_refGlade->get_widget("Work_Save_Button_1", m_pWorkButtonSaveObject[0]);
	if (m_pWorkButtonSaveObject[0])
		m_pWorkButtonSaveObject[0]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::work_on_button_object_save), 1));

	m_refGlade->get_widget("Work_Save_Button_2", m_pWorkButtonSaveObject[1]);
	if (m_pWorkButtonSaveObject[1])
		m_pWorkButtonSaveObject[1]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::work_on_button_object_save), 2));

	m_refGlade->get_widget("Work_Button_Id_1", m_pWorkButtonObjectId[0]);
	if (m_pWorkButtonObjectId[0])
		m_pWorkButtonObjectId[0]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::work_change_id), 1));

	m_refGlade->get_widget("Work_Button_Id_2", m_pWorkButtonObjectId[1]);
	if (m_pWorkButtonObjectId[1])
		m_pWorkButtonObjectId[1]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::work_change_id), 2));

	Glib::RefPtr<Gtk::CssProvider> css_provider_buttons = Gtk::CssProvider::create();
	css_provider_buttons->load_from_data(
	     "button:hover {background-image: image(#33D17A);}\
	      button:active {color: #FF0000; border-radius: 50px;}");

	m_refGlade->get_widget("Work_Button_Configuration", m_pWorkButtonConfiguration);
	if (m_pWorkButtonConfiguration)
		m_pWorkButtonConfiguration->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_on_button_configuration));

	m_refGlade->get_widget("Work_Button_Role", m_pWorkButtonRole);
	if (m_pWorkButtonRole)
		m_pWorkButtonRole->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_change_user));

	m_pCurrentReadyStatus = ReadyStatus::Ready;

	m_refGlade->get_widget("Work_Button_Ready", m_pWorkButtonReady);
	if (m_pWorkButtonReady)
		m_pWorkButtonReady->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_change_ready_status));

	m_refGlade->get_widget("Work_Button_Start", m_pWorkButtonStart);
	if (m_pWorkButtonStart)
		m_pWorkButtonStart->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_on_button_start));

	m_pWorkButtonConfiguration->get_style_context()->add_provider(css_provider_buttons, GTK_STYLE_PROVIDER_PRIORITY_USER);
	m_pWorkButtonRole->get_style_context()->add_provider(css_provider_buttons, GTK_STYLE_PROVIDER_PRIORITY_USER);
	m_pWorkButtonReady->get_style_context()->add_provider(css_provider_buttons, GTK_STYLE_PROVIDER_PRIORITY_USER);

	Glib::RefPtr<Gtk::CssProvider> css_provider_images = Gtk::CssProvider::create();
	css_provider_images->load_from_data("* {border: 2px solid red; }");

	for(int row = 1; row <= 2; row++)
	{
		for(int col = 1; col <= 8; col++)
		{
			m_refGlade->get_widget_derived("Image" + to_string(row) + "-" + to_string(col), m_pWorkDrawingArea[row - 1][col - 1]);
			m_refGlade->get_widget("Work_Button_Image" + to_string(row) + "-" + to_string(col), m_pWorkButtons[row - 1][col - 1]);
			m_refGlade->get_widget("Work_Frame" + to_string(row) + "-" + to_string(col), m_pWorkFrames[row - 1][col - 1]);
			m_pWorkFrames[row - 1][col - 1]->get_style_context()->add_provider(css_provider_images, GTK_STYLE_PROVIDER_PRIORITY_USER);
			m_pWorkButtons[row - 1][col - 1]->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::work_on_click_image), row, col - 1));
		}
	}

	work_hide_object(1);
	work_hide_object(2);

	// Settings
	m_refGlade->get_widget("Settings_Button_Work", m_pSettingsButtonWork);
	if (m_pSettingsButtonWork)
		m_pSettingsButtonWork->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Work));

	m_refGlade->get_widget("Settings_Button_Exposure", m_pSettingsButtonExposure);
	if (m_pSettingsButtonExposure)
		m_pSettingsButtonExposure->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::settings_on_button_exposure_change));

	m_refGlade->get_widget("Settings_Button_Configuration", m_pSettingsButtonConfiguration);
	if (m_pSettingsButtonConfiguration)
		m_pSettingsButtonConfiguration->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::work_on_button_configuration));

	// Log
	m_refGlade->get_widget("Log_Button_Back", m_pLogButtonBack);
	if (m_pLogButtonBack)
		m_pLogButtonBack->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Work));

	m_refGlade->get_widget("Log_Combo_Box", m_pLogComboBox);
	if (m_pLogComboBox)
		m_pLogComboBox->signal_changed().connect(sigc::mem_fun(*this, &MainWindow::log_on_changed_combo_box));

	m_refGlade->get_widget("Log_Text", m_pLogTextView);

	// Error
	m_refGlade->get_widget("Error_Button_Back", m_pErrorButtonBack);
	if (m_pErrorButtonBack)
		m_pErrorButtonBack->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Work));

	m_refGlade->get_widget("Error_Button_Reset", m_pErrorButtonReset);

	m_refGlade->get_widget("Error_List", m_pErrorTreeView);

	// Archive
	m_refGlade->get_widget("Archive_Button_Back", m_pArchiveButtonBack);
	if (m_pArchiveButtonBack)
		m_pArchiveButtonBack->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::SwitchMode), eSysMode::Work));

	m_refGlade->get_widget("Inspections_List", m_pArchiveTreeView);


	// Other
	SwitchMode(eSysMode::Init);

//	db.Open("192.168.0.251", "intro", "IntroTest", "introtest");

	m_pCurrentImageWidth = 190;
	m_pCurrentImageHeight = 160;
};

MainWindow::~MainWindow(void)
{
	if (m_pExpChangeDlg)
		m_pExpChangeDlg->close();
}

void MainWindow::on_window_hide(void)
{
	theDispatcher.Stop();
}

void MainWindow::SwitchMode(eSysMode mode){
	m_pButtonArchive->hide();
	m_pButtonLog->hide();
	m_pButtonError->hide();
	switch(mode){
	case eSysMode::Init:
		m_pStack->set_visible_child("Arm_Init");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#e5e4e2"));
		m_pLabelStatus->set_text("Инициализация");
		this->set_size_request(600, 500);
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Init") });
		break;
	case eSysMode::Work:
		m_pButtonArchive->show();
		m_pButtonLog->show();
		m_pButtonError->show();
		m_pStack->set_visible_child("Arm_Work");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#8FBC8F"));
		m_pLabelStatus->set_text("Рабочий режим");
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Work") });
		break;
	case eSysMode::Settings:
		if (!this->work_check_is_object_saved())
			break;
		m_pStack->set_visible_child("Arm_Settings");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#FFFF00"));
		m_pLabelStatus->set_text("Режим настройки");
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Configuration") });
		break;
	case eSysMode::Error:
		m_pStack->set_visible_child("Arm_Error");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#ff2400"));
		m_pLabelStatus->set_text("Режим просмотра ошибок");
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Error") });
		break;
	case eSysMode::Archive:
		m_pStack->set_visible_child("Arm_Archive");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#e59866"));
		m_pLabelStatus->set_text("Режим архива");
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Archive") });
		break;
	case eSysMode::Log:
		m_pStack->set_visible_child("Arm_Log");
		m_pBoxStatus->override_background_color(Gdk::RGBA("#FFDDCA"));
		m_pLabelStatus->set_text("Просмотр лог файла");
		theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Log") });
		break;
	default:
		break;
	}
}

void MainWindow::init_on_button_enter(void){
	if(!init_check_login(this->m_pInitEntryLogin->get_text()))
	{
		this->m_pInitEntryLogin->grab_focus();
		this->m_pInitLabelError->show();
		return;
	}

	if (!init_check_password(this->m_pInitEntryPassword->get_text()))
	{
		this->m_pInitEntryPassword->grab_focus();
		this->m_pInitLabelError->show();
		return;
	}

	this->SwitchMode(eSysMode::Work);
	this->m_pInitEntryPassword->set_text("");
	this->m_pInitEntryLogin->set_text("");
}

bool MainWindow::init_check_login(string login){
	return true;
//	return db.ContainsUserLogin(login) == 1;
}

bool MainWindow::init_check_password(string password){
	return true;
//	return db.ContainsUserPassword(md5(password)) == 1;
}

void MainWindow::init_on_entry_changed(void){
	m_pInitLabelError->hide();
}

void MainWindow::on_button_about(void){
	AngaraAboutDialog* pDialog = nullptr;
	m_refGlade->get_widget_derived("App_About_Dialog", pDialog);
	if (pDialog)
	{
		pDialog->set_transient_for(*this);
		pDialog->set_modal();

		pDialog->run();
	}
}
void MainWindow::on_button_quit(void){
	Gtk::MessageDialog* pDialog = nullptr;
	m_refGlade->get_widget("Close_Confirmation_Dialog", pDialog);
	if (! pDialog) return;

	if (pDialog->run() != Gtk::RESPONSE_OK)
	{
		pDialog->close();
		return;
	}

	pDialog->close();

	hide();
}

void MainWindow::settings_on_button_exposure_change(void){
	if (! m_pExpChangeDlg)
	{
		m_refGlade->get_widget_derived("Exposure_Change_Dialog", m_pExpChangeDlg);
		if (! m_pExpChangeDlg) return;
	}

	m_pExpChangeDlg->run();
}

void MainWindow::work_on_button_object_save(int post)
{
	if(this->work_check_object_id(post))
	{
		Gtk::MessageDialog* pDialog = nullptr;
		m_refGlade->get_widget("Contains_Object_Id_Dialog", pDialog);
		if (! pDialog) return;

		if (pDialog->run() != Gtk::RESPONSE_YES)
		{
			pDialog->close();
			return;
		}

		pDialog->close();
	}

	work_save_object(post);
}

void MainWindow::log_update_log(string path){
	fstream log;
	log.open(path, ios::in);
	string result = "", temp;
	if (log.is_open()){
		while(getline(log, temp)){
			result += temp + "\n";
		}
		log.close();
	}

	auto buffer = m_pLogTextView->get_buffer();

	buffer.get()->set_text(result);
}

void MainWindow::work_change_id(int index){
	index--;
	OpenObjectDialog* pDialog = nullptr;
	m_refGlade->get_widget_derived("Object_Open_Dialog", pDialog);
	if (! pDialog) return;

	if (pDialog->run() != Gtk::RESPONSE_OK)
	{
		pDialog->close();
		return;
	}

	pDialog->close();

	m_pObjectId[index] = pDialog->GetNewID();

	m_pWorkButtonObjectId[index]->set_label(to_string(m_pObjectId[index]));
}

void MainWindow::log_on_changed_combo_box(void){
	string selected = this->m_pLogComboBox->get_active_id();
	if (selected == "Arm")
		log_update_log("log-arm.txt");
	else if (selected == "Controller")
		log_update_log("log-controller.txt");
	else if (selected == "Kernel")
		log_update_log("log-kernel.txt");
}

void MainWindow::work_on_button_configuration(void){
	if (!this->work_check_is_object_saved())
		return;

	string configuration;
	ConfigureDialog* pConfigureDialog = nullptr;
	m_refGlade->get_widget_derived("Configure_Dialog", pConfigureDialog);
	if (! pConfigureDialog) return;

	if (pConfigureDialog)
	{
		pConfigureDialog->SetConfigure(&configuration);
		pConfigureDialog->set_transient_for(*this);
		pConfigureDialog->set_modal();
		pConfigureDialog->run();
	}

	pConfigureDialog->close();

	if (configuration != ""){
		configuration = " " + configuration + " ";
		m_pWorkLabelConfiguration->set_label(configuration);
	}
}

void MainWindow::work_on_click_image(int post, int index){
	int status = -1;
	cv::Mat currentImage = m_pWorkImages[post - 1][index];
	ImageDialog* pDialog = nullptr;
	m_refGlade->get_widget_derived("Current_Image_Dialog", pDialog);

	if (pDialog)
	{
		pDialog->SetStatus(&status);
		pDialog->UpdateImage(currentImage);
		pDialog->set_transient_for(*this);
		pDialog->set_modal();
		pDialog->run();
	}

	if (status != -1)
	{
		bool bl_status = status == 0? false : true;
		this->m_pObjectStatus[post - 1][index] = bl_status;
		this->work_switch_image_status(post, index, bl_status);
		this->work_switch_object_status();
	}
}

void MainWindow::work_switch_image_status(int post, int index, bool status){
	string css_str = status? "* {border: 2px solid green; }" : "* {border: 2px solid red; }";
	Glib::RefPtr<Gtk::CssProvider> css_provider_status = Gtk::CssProvider::create();
	css_provider_status->load_from_data(css_str);
	this->m_pWorkFrames[post - 1][index]->get_style_context()->add_provider(css_provider_status, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

bool MainWindow::work_check_object_status(int post){
	for(int i = 0; i < this->m_pImageCount[post - 1]; i++)
		if (!this->m_pObjectStatus[post - 1][i])
			return false;
	return true;
}

bool MainWindow::work_check_object_id(int post){
	return true;
//	return this->db.ContainsObjectId(m_pWorkButtonObjectId[post - 1]->get_label()) == 1;
}

void MainWindow::work_switch_object_status(void){
	Pango::Attribute red = Pango::Attribute::create_attr_background(65535, 0, 0);
	Pango::Attribute green = Pango::Attribute::create_attr_background(9766, 41634, 26985);
	Pango::AttrList attr_red, attr_green;
	attr_red.insert(red);
	attr_green.insert(green);

	for(int i = 0; i < 2; i++)
	{
		this->m_pWorkLabelStatus[i]->set_attributes(work_check_object_status(i + 1) ? attr_green : attr_red);
		this->m_pWorkLabelStatus[i]->set_text(work_check_object_status(i + 1) ? " Годен ": " Дефект ");
	}
}

void MainWindow::work_set_image(int post, int index){
	post--;
	cv::Mat resized_image;
	cv::resize(m_pWorkImages[post][index], resized_image, cv::Size(this->m_pCurrentImageWidth, this->m_pCurrentImageHeight), cv::INTER_LINEAR);
	m_pWorkDrawingArea[post][index]->set_size_request(this->m_pCurrentImageWidth, this->m_pCurrentImageHeight);
	m_pWorkDrawingArea[post][index]->UpdateImage(resized_image, true);
	m_pWorkDrawingArea[post][index]->queue_draw();
	m_pWorkButtons[post][index]->show();
	m_pImageCount[post]++;
}

void MainWindow::work_hide_object(int post){
	post--;
	for(int i = 0; i < 8; i++)
		m_pWorkButtons[post][i]->hide();

	m_pWorkButtonSaveObject[post]->hide();
	m_pWorkLabelStatus[post]->hide();

	m_pWorkButtonObjectId[post]->set_label("#######");
	m_pWorkButtonObjectId[post]->set_sensitive(false);
}

void MainWindow::work_show_object(int post){
	post--;
	for(int i = 0; i < m_pImageCount[post]; i++)
		m_pWorkButtons[post][i]->show();

	m_pWorkButtonSaveObject[post]->show();
	m_pWorkLabelStatus[post]->show();

	m_pWorkButtonObjectId[post]->set_label("000000");
	m_pWorkButtonObjectId[post]->set_sensitive(true);
}

void MainWindow::work_save_object(int post){
	m_pIsObjectSaved[post - 1] = true;
	work_hide_object(post);

	for(int i = 0; i < 8; i++)
	{
		m_pObjectStatus[post - 1][i] = false;
		this->work_switch_image_status(post, i, false);
	}

	this->work_switch_object_status();

//	theDispatcher.SendMessage(TrgKernelLink, { TrgGui, CmdSend, std::string("ImageSave") });

	if (this->m_pIsObjectSaved[0] && this->m_pIsObjectSaved[1])
		m_pWorkButtonStart->show();
}

bool MainWindow::work_check_is_object_saved(void){
	if(!this->m_pIsObjectSaved[0] || !this->m_pIsObjectSaved[1])
	{
		Gtk::MessageDialog* pDialog = nullptr;
		m_refGlade->get_widget("Save_Object_Dialog", pDialog);
		if (! pDialog) return false;

		if (pDialog->run() != Gtk::RESPONSE_OK)
		{
			pDialog->hide();
			pDialog->close();
			return false;
		}

		pDialog->hide();
		pDialog->close();

		this->work_save_object(1);
		this->work_save_object(2);
	}
	return true;
}

void MainWindow::work_on_button_start(void){
	Gtk::MessageDialog* pDialog = nullptr;
	m_refGlade->get_widget("Object_Ready_Dialog", pDialog);
	if (! pDialog) return;

	if (pDialog->run() != Gtk::RESPONSE_YES)
	{
		pDialog->close();
		return;
	}

	pDialog->close();

	work_load_images();
}

void MainWindow::work_load_images(void){
	m_pImageCount[0] = 0;
    m_pImageCount[1] = 0;

    m_pIsObjectSaved[0] = false;
	m_pIsObjectSaved[1] = false;

	for(int i = 0; i < 2; i++)
		for(int j = 0; j < 8; j++)
			m_pObjectStatus[i][j] = false;

	cv::Mat image;
	for(int i = 0; i < 8; i++)
	{
		m_pWorkImages[0][i] = cv::imread("Images/ViTecLogo.png", cv::IMREAD_COLOR);
		m_pWorkImages[1][i] = cv::imread("Images/ViTecLogoNeg.png", cv::IMREAD_COLOR);
		this->work_set_image(1, i);
		this->work_set_image(2, i);
	}

	work_show_object(1);
	work_show_object(2);

	work_switch_object_status();

	m_pWorkButtonStart->hide();
}

void MainWindow::work_change_ready_status(void){
	//TODO delete
	m_pCurrentReadyStatus = m_pCurrentReadyStatus == ReadyStatus::Ready? ReadyStatus::Wait : m_pCurrentReadyStatus == ReadyStatus::Wait ? ReadyStatus::Err : ReadyStatus::Ready;
	//

	Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
	Pango::Attribute color, variant = Pango::Attribute::create_attr_variant(Pango::Variant::VARIANT_SMALL_CAPS);
	Pango::AttrList attr;
	string text, css;

	switch(m_pCurrentReadyStatus){
	case ReadyStatus::Ready:
		color = Pango::Attribute::create_attr_background(51 * 247, 209 * 247, 122 * 247);
		text = " Готов к работе ";
		css = "button:hover {background-image: image(#31c975);}\
			   button:active {color: #FF0000; border-radius: 50px;}";
		break;
	case ReadyStatus::Wait:
		color = Pango::Attribute::create_attr_background(255 * 247, 102 * 247, 0);
		text = " Ожидание ";
		css = "button:hover {background-image: image(#f66200);}\
			   button:active {color: #000000; border-radius: 50px;}";
		break;
	case ReadyStatus::Err:
		color = Pango::Attribute::create_attr_background(255 * 247, 0, 0);
		text = " Ошибка ";
		css = "button:hover {background-image: image(#f60000);}\
			   button:active {color: #000000; border-radius: 50px;}";
		break;
	default:
		break;
	}

	attr.insert(color);
	attr.insert(variant);

	this->m_pWorkLabelReady->set_attributes(attr);
	this->m_pWorkLabelReady->set_text(text);

	css_provider->load_from_data(css);

	m_pWorkButtonReady->get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void MainWindow::work_change_user(void){
	Gtk::MessageDialog* pDialog = nullptr;
	m_refGlade->get_widget("Change_User_Dialog", pDialog);
	if (! pDialog) return;

	if (pDialog->run() != Gtk::RESPONSE_YES)
	{
		pDialog->close();
		return;
	}

	pDialog->close();

	this->SwitchMode(eSysMode::Init);
}

void MainWindow::work_on_close_button(void){
 	Gtk::MessageDialog* pDialog = nullptr;
	m_refGlade->get_widget("Exit_Confirmation_Dialog", pDialog);
	if (! pDialog) return;

	if (pDialog->run() != Gtk::RESPONSE_OK)
	{
		pDialog->close();
		return;
	}

	pDialog->close();

	theDispatcher.SendMessage(TrgControllerLink, { TrgGui, CmdSend, std::string("Exit") });
}


gboolean MainWindow::MsgProcess(gpointer data)
{
	MainWindow *pWnd = (MainWindow *)data;
	if (! pWnd)
		return false;

	DispMessage sMessage = pWnd->GetMessage();
	if (sMessage.m_Cmd != CmdUnknown)
	{
		// Process the message queue
		pWnd->MsgProc(sMessage);
	}
	else
	{
		// Sleep a moment if no messages
		usleep(3000);
	}

	return ! pWnd->IsMsgStopped();
}

void MainWindow::MsgProc(DispMessage sMessage)
{
	std::cout << GetMessageName(sMessage) << " received in GUI" << std::endl;

	string strMessage = sMessage.m_Params;
	std::vector<std::string> vsTokens = SplitString(strMessage, CH_CMD_DELIM);

	int iLink = 0;

	switch (sMessage.m_Cmd)
	{
	case CmdStatus:
	case CmdObject:
	case CmdObjOpen:
	case CmdObjClosed:
	case CmdObjResult:
	case CmdConfigUse:
	case CmdFrame:
	case CmdPreview:
	case CmdExposure:
	case CmdImageSaved:
	case CmdConfig:
	case CommStatus:
	default:
		break;
	}
}
