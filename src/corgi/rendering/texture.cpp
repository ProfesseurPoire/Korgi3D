#include "texture.h"

#include <corgi/rendering/RenderCommand.h>
#include <corgi/logger/log.h>

#include <corgi/rapidjson/rapidjson.h>
#include <corgi/rapidjson/document.h>
#include <corgi/rapidjson/filereadstream.h>

#include <corgi/utils/Utils.h>

using namespace corgi;

Texture::Texture()
{
	log_info("Creating new empty texture");
}

static Texture::MagFilter parse_mag_filter(const std::string& str)
{
	return mag_filters.at(str);
}

static std::map<std::string, Texture::MagFilter> mag_filters =
{
	{"nearest", Texture::MagFilter::Nearest},
	{"linear", Texture::MagFilter::Linear}
};

static std::map<std::string, Texture::Wrap> wraps = 
{
	{"repeat", Texture::Wrap::Repeat},
	{"clamp_to_border", Texture::Wrap::ClampToBorder},
	{"clamp_to_edge", Texture::Wrap::ClampToEdge },
	{"mirrored_repeat", Texture::Wrap::MirroredRepeat},
	{"mirror_clamp_to_edge", Texture::Wrap::MirrorClampToEdge}
};
	
static Texture::Wrap load_wrap(const std::string& str)
{
	return wraps.at(str);
}

static std::map<std::string, Texture::MinFilter> min_filters_ =
{
	{"nearest", Texture::MinFilter::Nearest},
	{"linear", Texture::MinFilter::Linear},
	{"nearest_mipmap_nearest", Texture::MinFilter::NearestMipmapNearest},
	{"nearest_mipmap_linear", Texture::MinFilter::NearestMipmapLinear},
	{"linear_mipmap_linear", Texture::MinFilter::LinearMipmapLinear},
	{"linear_mipmap_nearest", Texture::MinFilter::LinearMipmapNearest}
};

static Texture::MinFilter parse_min_filter(const std::string& str)
{
	return min_filters_.at(str);
}

Texture::Texture(const std::string& path)
{
	if (!path.empty())
	{
		// So apparently this is where I actually 
		std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

		int fileSize = 0;

		if(!file.is_open())
		{
			throw("Could not open file for texture");
		}

		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			fileSize = int(file.tellg());
			file.close();
		}

		FILE* fp = fopen(path.c_str(), "rb"); // non-Windows use "r"

		char* readBuffer = new char[fileSize];

		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		rapidjson::Document document;
		document.ParseStream(is);

		assert(document.HasMember("wrap_s"));
		assert(document.HasMember("wrap_t"));
		assert(document.HasMember("min_filter"));
		assert(document.HasMember("mag_filter"));

		corgi::Image* image = Editor::Utils::LoadImageForReal(
			(path.substr(0, path.size() - 4) + ".png").c_str());

		name_ 	= filesystem::filename((path.substr(0, path.size() - 4) + ".png"));
		width_ 	= image->width();
		height_ = image->height();
		
		min_filter_ = parse_min_filter(document["min_filter"].GetString());
		mag_filter_ = parse_mag_filter(document["mag_filter"].GetString());

		wrap_s_ 	= load_wrap(document["wrap_s"].GetString());
		wrap_t_		= load_wrap(document["wrap_t"].GetString());

		id_ 		= RenderCommand::generate_texture_object();

		RenderCommand::bind_texture_object(id_);

		if (image->channel() == 3)
		{
			RenderCommand::initialize_texture_object
			(
				Format::RGB, InternalFormat::RGB,
				width_, height_,
				DataType::UnsignedByte,
				image->pixels()
			);
		}

		if (image->channel() == 4)
		{
			RenderCommand::initialize_texture_object
			(
				Format::RGBA, InternalFormat::RGBA,
				width_, height_,
				DataType::UnsignedByte,
				image->pixels()
			);
		}

		RenderCommand::texture_parameter(min_filter_);
		RenderCommand::texture_parameter(mag_filter_);
		RenderCommand::texture_wrap_s(wrap_s_);
		RenderCommand::texture_wrap_t(wrap_t_);
		RenderCommand::end_texture();	

		delete image;
	}
	else
	{
		throw("Could not construstruct the thing");
	}
	
}

Texture::Texture(Texture&& texture) noexcept
	:
		name_(std::move(texture.name_)),
		id_(texture.id_),
		min_filter_(texture.min_filter_),
		mag_filter_(texture.mag_filter_),
		wrap_s_(texture.wrap_s_),
		wrap_t_(texture.wrap_t_),
		width_(texture.width_),
		height_(texture.height_)
{
	log_info("Texture Move Constructor for "+ name_);

	texture.id_			= 0u;
	texture.width_		= static_cast<unsigned short>(0);
	texture.height_		= static_cast<unsigned short>(0);
	texture.min_filter_ = MinFilter::Nearest;
	texture.mag_filter_ = MagFilter::Nearest;
	texture.wrap_s_		= Wrap::Repeat;
	texture.wrap_t_		= Wrap::Repeat;
}

Texture& Texture::operator=(Texture&& texture) noexcept
{
	log_info("Move Affectation texture for "+ name_);

	if(id_!=0)
		RenderCommand::delete_texture_object(id_);

	name_		= std::move(texture.name_);
	id_			= texture.id_;
	min_filter_ = texture.min_filter_;
	mag_filter_ = texture.mag_filter_;
	wrap_s_		= texture.wrap_s_;
	wrap_t_		= texture.wrap_t_;
	width_		= texture.width_;
	height_		= texture.height_;

	texture.id_			= 0u;
	texture.width_		= static_cast<unsigned short>(0);
	texture.height_		= static_cast<unsigned short>(0);
	texture.min_filter_ = MinFilter::Nearest;
	texture.mag_filter_ = MagFilter::Nearest;
	texture.wrap_s_		= Wrap::Repeat;
	texture.wrap_t_		= Wrap::Repeat;

	return *this;
}

const std::string& Texture::name() const
{
	return name_;
}

Texture::Texture(
	const std::string& name,
	int width, int height,
	MinFilter min_f, MagFilter mag_f,
	Wrap wrap_s, Wrap wrap_t,
	Format format, InternalFormat internal_format,
	DataType dt, unsigned char* data) 
	:	name_(name),
		id_(RenderCommand::generate_texture_object()),
		min_filter_(min_f),
		mag_filter_(mag_f),
		wrap_s_(wrap_s),
		wrap_t_(wrap_t),
		width_(static_cast<unsigned short>(width)),
		height_(static_cast<unsigned short>(height))
	{

	log_info("Texture Constructor for "+name);

	RenderCommand::bind_texture_object(id_);
	RenderCommand::initialize_texture_object
	(
		format,
		internal_format,
		width,
		height,
		dt,
		data
	);

	RenderCommand::texture_parameter(min_f);
	RenderCommand::texture_parameter(mag_f);
	RenderCommand::texture_wrap_s(wrap_s);
	RenderCommand::texture_wrap_t(wrap_t);
	RenderCommand::end_texture();
}

Texture::~Texture()
{
	log_info("Texture Destructor for "+name_);
	RenderCommand::delete_texture_object(id_);
}

void Texture::apply_changes()
{
	if (id_ == 0)
		log_warning("Could not apply texture's changes");

	RenderCommand::bind_texture_object(id_);
	RenderCommand::texture_parameter(min_filter_);
	RenderCommand::texture_parameter(mag_filter_);
	RenderCommand::texture_wrap_s(wrap_s_);
	RenderCommand::texture_wrap_t(wrap_t_);
	RenderCommand::end_texture();
}

unsigned Texture::id() const noexcept
{
	return id_;
}

Texture::MinFilter Texture::min_filter() const noexcept
{
	return min_filter_;
}

Texture::MagFilter Texture::mag_filter() const noexcept
{
	return mag_filter_;
}

Texture::Wrap Texture::wrap_s() const noexcept
{
	return wrap_s_;
}

Texture::Wrap Texture::wrap_t() const noexcept
{
	return wrap_t_;
}

int Texture::width()const noexcept
{
	return static_cast<int>(width_);
}

int Texture::height()const noexcept
{
	return static_cast<int>(height_);
}

void Texture::width(int width)
{
	width_ = static_cast<unsigned short>(width);
}

void Texture::height(int height)
{
	height_ = static_cast<unsigned short>(height);
}

//
//
//static corgi::containers::std::vector<std::string> wraps
//{
//    "clamp_to_border",
//    "clamp_to_edge",
//    "mirrored_repeat",
//    "mirror_clamp_to_edge",
//    "repeat"
//};
//
//static Vector<std::string> min_filters
//{
//    "nearest",
//    "linear",
//    "nearest_mipmap_nearest",
//    "nearest_mipmap_linear",
//    "linear_mipmap_linear",
//    "linear_mipmap_nearest"
//};
//
//static Vector<std::string> mag_filters
//{
//    "nearest",
//    "linear"
//};
//
//// If str exist inside strings, returns its index
//int get_index(Vector<std::string>& strings, const std::string& str)
//{
//    const auto iterator = std::find(strings.begin(), strings.end(), str);
//
//    if (iterator == strings.end())
//    {
//       // log_warning("Could not find" + str + " index inside the Vector");
//        return 0;
//    }
//    return std::distance(strings.begin(), iterator);
//}
//
//static std::string min_filter_to_string(Texture::MinFilter filter)
//{
//    return min_filters[static_cast<int>(filter)];
//}
//
//static Texture::MinFilter string_to_min_filter(const std::string& str)
//{
//    return static_cast<Texture::MinFilter>(get_index(min_filters, str));
//}
//
//static std::string mag_filter_to_string(Texture::MagFilter filter)
//{
//    return mag_filters[static_cast<int>(filter)];
//}
//
//static Texture::MagFilter string_to_mag_filter(const std::string& str)
//{
//    return static_cast<Texture::MagFilter>(get_index(mag_filters, str));
//}
//
//static std::string wrap_to_string(Texture::Wrap wrp)
//{
//    return wraps[static_cast<int>(wrp)];
//}
//
//static Texture::Wrap string_to_wrap(const std::string& str)
//{
//    return static_cast<Texture::Wrap>(get_index(wraps, str));
//}
//
//void Texture::load( const std::string& file)
//{
//    //_name = file;
//
//    //std::string path = ResourceFolder::instance()->folder() + "/images/" + file;
//
//    //// We also try to load the .asset file. If it doesn't exist, it gets
//    //// created
//
//    //filesystem::ini::Document doc;
//    //ConstString asset_path = path + ".asset";
//
//    //if (filesystem::file_exist(asset_path))
//    //{
//    //    doc.read(asset_path);
//
//    //    _wrap_s = string_to_wrap(doc.value("wrap_s", "wrap").as_string());
//    //    _wrap_t = string_to_wrap(doc.value("wrap_t", "wrap").as_string());
//    //    _min_filter = string_to_min_filter(doc.value("min_filter", "filters").as_string());
//    //    _mag_filter = string_to_mag_filter(doc.value("mag_filter", "filters").as_string());
//
//    //}
//    //else  // No asset file as been founded, default one created
//    //{
//    //    doc.set_value(wrap_to_string(_wrap_s), "wrap_s", "wrap");
//    //    doc.set_value(wrap_to_string(_wrap_t), "wrap_t", "wrap");
//    //    doc.set_value(min_filter_to_string(_min_filter), "min_filter", "filters");
//    //    doc.set_value(mag_filter_to_string(_mag_filter), "mag_filter", "filters");
//
//    //    doc.write(asset_path);
//    //}
//
//    //Image img;
//    //img.load_from_png_file(path.c_str());
//    //image(img);
//}


void Texture::min_filter(MinFilter filter)noexcept
{
	min_filter_ = filter;
}

void Texture::mag_filter(MagFilter filter)noexcept
{
	mag_filter_ = filter;
}

void Texture::wrap_s(Wrap wrap) noexcept
{
	wrap_s_ = wrap;
}

void Texture::wrap_t(Wrap wrap)noexcept
{
	wrap_t_ = wrap;
}


float Texture::ratio() const
{
	return  static_cast<float>(width_) / static_cast<float>(height_);
}


void Texture::unpack_pixels()
{
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}


void Texture::build_texture(int width, int height)
{
   /* width_ = width;
	_height = height;

	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width_,
		_height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		0
	);
	check_gl_error();*/
}