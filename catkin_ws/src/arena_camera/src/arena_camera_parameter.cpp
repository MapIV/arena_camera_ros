/******************************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright (C) 2016, Magazino GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the names of Magazino GmbH nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <arena_camera/arena_camera_parameter.h>
#include <sensor_msgs/image_encodings.h>

namespace arena_camera
{
  ArenaCameraParameter::ArenaCameraParameter()
      : camera_frame_("arena_camera"),
        device_user_id_(""),
        frame_rate_(5.0),
        camera_info_url_(""),
        image_encoding_(""),
        image_encoding_given_(false),
        binning_x_(1),
        binning_y_(1),
        binning_x_given_(false),
        binning_y_given_(false),
        downsampling_factor_exp_search_(1),
        // ##########################
        //  image intensity settings
        // ##########################
        exposure_(10000.0),
        exposure_given_(false),
        gain_(0.5),
        gain_given_(false),
        gamma_(1.0),
        gamma_given_(false),
        brightness_(100),
        brightness_given_(false),
        brightness_continuous_(false),
        exposure_auto_(true),
        gain_auto_(true),
        // #########################
        exposure_search_timeout_(5.),
        auto_exp_upper_lim_(0.0),
        mtu_size_(3000),
        inter_pkg_delay_(1000),
        shutter_mode_(SM_DEFAULT),
        auto_flash_(false),
        // #########################
        ptp_enable_(true),
        trigger_mode_("Off"),
        trigger_source_("Software"),
        trigger_selector_("FrameStart"),
        width_(1080),
        height_(1920)

  {
  }

  ArenaCameraParameter::~ArenaCameraParameter()
  {
  }

  void ArenaCameraParameter::readFromRosParameterServer(const ros::NodeHandle &nh)
  {
    nh.param<std::string>("camera_frame", camera_frame_, "arena_camera");

    nh.param<std::string>("device_user_id", device_user_id_, "");

    if (nh.hasParam("frame_rate"))
    {
      nh.getParam("frame_rate", frame_rate_);
    }

    nh.param<std::string>("camera_info_url", camera_info_url_, "");
    if (nh.hasParam("camera_info_url"))
    {
      nh.getParam("camera_info_url", camera_info_url_);
    }

    binning_x_given_ = nh.hasParam("binning_x");
    if (binning_x_given_)
    {
      int binning_x;
      nh.getParam("binning_x", binning_x);
      std::cout << "binning x is given and has value " << binning_x << std::endl;
      if (binning_x > 32 || binning_x < 0)
      {
        ROS_WARN_STREAM("Desired horizontal binning_x factor not in valid "
                        << "range! Binning x = " << binning_x << ". Will reset it to "
                        << "default value (1)");
        binning_x_given_ = false;
      }
      else
      {
        binning_x_ = static_cast<size_t>(binning_x);
      }
    }
    binning_y_given_ = nh.hasParam("binning_y");
    if (binning_y_given_)
    {
      int binning_y;
      nh.getParam("binning_y", binning_y);
      std::cout << "binning y is given and has value " << binning_y << std::endl;
      if (binning_y > 32 || binning_y < 0)
      {
        ROS_WARN_STREAM("Desired vertical binning_y factor not in valid "
                        << "range! Binning y = " << binning_y << ". Will reset it to "
                        << "default value (1)");
        binning_y_given_ = false;
      }
      else
      {
        binning_y_ = static_cast<size_t>(binning_y);
      }
    }
    nh.param<int>("downsampling_factor_exposure_search", downsampling_factor_exp_search_, 20);
    image_encoding_given_ = nh.hasParam("image_encoding");
    if (nh.hasParam("image_encoding"))
    {
      std::string encoding;
      nh.getParam("image_encoding", encoding);
      // if (!encoding.empty() &&
      // 	!sensor_msgs::image_encodings::isMono(encoding) &&
      // 	!sensor_msgs::image_encodings::isColor(encoding) &&
      // 	!sensor_msgs::image_encodings::isBayer(encoding) &&
      // 	encoding != sensor_msgs::image_encodings::YUV422)
      // {
      // 	ROS_WARN_STREAM("Desired image encoding parameter: '" << encoding
      // 														  << "' is not part of the 'sensor_msgs/image_encodings.h'
      // list!"
      // 														  << " Will not set encoding");
      // 	encoding = std::string("");
      // }
      image_encoding_ = encoding;
    }

    // ##########################
    //  image intensity settings
    // ##########################

    // > 0: Exposure time in microseconds
    exposure_given_ = nh.hasParam("exposure");
    if (exposure_given_)
    {
      nh.getParam("exposure", exposure_);
      std::cout << "exposure is given and has value " << exposure_ << std::endl;
    }

    gain_given_ = nh.hasParam("gain");
    if (gain_given_)
    {
      nh.getParam("gain", gain_);
      std::cout << "gain is given and has value " << gain_ << std::endl;
    }

    gamma_given_ = nh.hasParam("gamma");
    if (gamma_given_)
    {
      nh.getParam("gamma", gamma_);
      std::cout << "gamma is given and has value " << gamma_ << std::endl;
    }

    brightness_given_ = nh.hasParam("brightness");
    if (brightness_given_)
    {
      nh.getParam("brightness", brightness_);
      std::cout << "brightness is given and has value " << brightness_ << std::endl;
      if (gain_given_ && exposure_given_)
      {
        ROS_WARN_STREAM("Gain ('gain') and Exposure Time ('exposure') "
                        << "are given as startup ros-parameter and hence assumed to be "
                        << "fix! The desired brightness (" << brightness_ << ") can't "
                        << "be reached! Will ignore the brightness by only "
                        << "setting gain and exposure . . .");
        brightness_given_ = false;
      }
      else
      {
        if (nh.hasParam("brightness_continuous"))
        {
          nh.getParam("brightness_continuous", brightness_continuous_);
          std::cout << "brightness is continuous" << std::endl;
        }
        if (nh.hasParam("exposure_auto"))
        {
          nh.getParam("exposure_auto", exposure_auto_);
          std::cout << "exposure is set to auto" << std::endl;
        }
        if (nh.hasParam("gain_auto"))
        {
          nh.getParam("gain_auto", gain_auto_);
          std::cout << "gain is set to auto" << std::endl;
        }
      }
    }
    // ##########################

    nh.param<double>("exposure_search_timeout", exposure_search_timeout_, 5.);
    nh.param<double>("auto_exposure_upper_limit", auto_exp_upper_lim_, 10000000.);

    if (nh.hasParam("gige/mtu_size"))
    {
      nh.getParam("gige/mtu_size", mtu_size_);
    }

    if (nh.hasParam("gige/inter_pkg_delay"))
    {
      nh.getParam("gige/inter_pkg_delay", inter_pkg_delay_);
    }

    std::string shutter_param_string;
    nh.param<std::string>("shutter_mode", shutter_param_string, "");
    if (shutter_param_string == "rolling")
    {
      shutter_mode_ = SM_ROLLING;
    }
    else if (shutter_param_string == "global")
    {
      shutter_mode_ = SM_GLOBAL;
    }
    else if (shutter_param_string == "global_reset")
    {
      shutter_mode_ = SM_GLOBAL_RESET_RELEASE;
    }
    else
    {
      shutter_mode_ = SM_DEFAULT;
    }

    nh.param<bool>("auto_flash", auto_flash_, false);
    nh.param<bool>("auto_flash_line_2", auto_flash_line_2_, true);
    nh.param<bool>("auto_flash_line_3", auto_flash_line_3_, true);

    ROS_WARN("Autoflash: %i, line2: %i , line3: %i ", auto_flash_, auto_flash_line_2_, auto_flash_line_3_);

    nh.param<bool>("ptp_enable", ptp_enable_, true);

    nh.param<std::string>("trigger_mode", trigger_mode_, "Off");
    nh.param<std::string>("trigger_source", trigger_source_, "Software");
    nh.param<std::string>("trigger_selector", trigger_selector_, "FrameStart");

    nh.getParam("width", width_);
    nh.getParam("height", height_);

    nh.getParam("aoi_enable", aoi_enable_);
    nh.getParam("aoi_width", aoi_width_);
    nh.getParam("aoi_height", aoi_height_);
    nh.getParam("aoi_offsetX", aoi_offsetX_);
    nh.getParam("aoi_offsetY", aoi_offsetY_);

    nh.getParam("exposure_auto_limit_auto", exposure_auto_limit_auto_);
    nh.getParam("exposure_auto_lower_limit", exposure_auto_lower_limit_);
    nh.getParam("exposure_auto_upper_limit", exposure_auto_upper_limit_);
    nh.getParam("exposure_auto_algorithm", exposure_auto_algorithm_);
    nh.getParam("exposure_auto_damping", exposure_auto_damping_);
    nh.getParam("exposure_auto_damping_raw", exposure_auto_damping_raw_);

    nh.getParam("hdr_output", hdr_output_);
    nh.getParam("hdr_tuning_enable", hdr_tuning_enable_);
    nh.getParam("hdr_tuning_channel_selector", hdr_tuning_channel_selector_);
    nh.getParam("hdr_channel_exposure_time", hdr_channel_exposure_time_);
    nh.getParam("hdr_channel_analog_gain", hdr_channel_analog_gain_);
    nh.getParam("hdr_digital_clamping_enable", hdr_digital_clamping_enable_);
    nh.getParam("hdr_image_enhancement_enable",hdr_image_enhancement_enable_);

    nh.getParam("acquisition_frame_rate_enable", acquisition_frame_rate_enable_);
    nh.getParam("acquisition_frame_rate", acquisition_frame_rate_);

    validateParameterSet(nh);
    return;
  }

  void ArenaCameraParameter::adaptDeviceUserId(const ros::NodeHandle &nh, const std::string &device_user_id)
  {
    device_user_id_ = device_user_id;
    nh.setParam("device_user_id", device_user_id_);
  }

  void ArenaCameraParameter::validateParameterSet(const ros::NodeHandle &nh)
  {
    if (!device_user_id_.empty())
    {
      ROS_INFO_STREAM("Trying to open the following camera: " << device_user_id_.c_str());
    }
    else
    {
      ROS_INFO_STREAM("No Device User ID set -> Will open the camera device "
                      << "found first");
    }

    if (frame_rate_ < 0 && frame_rate_ != -1)
    {
      ROS_WARN_STREAM("Unexpected frame rate (" << frame_rate_ << "). Will "
                                                << "reset it to default value which is 5 Hz");
      frame_rate_ = 5.0;
      nh.setParam("frame_rate", frame_rate_);
    }

    if (exposure_given_ && (exposure_ <= 0.0 || exposure_ > 1e7))
    {
      ROS_WARN_STREAM("Desired exposure measured in microseconds not in "
                      << "valid range! Exposure time = " << exposure_ << ". Will "
                      << "reset it to default value!");
      exposure_given_ = false;
    }

    if (gain_given_ && (gain_ < 0.0 || gain_ > 1.0))
    {
      ROS_WARN_STREAM("Desired gain (in percent) not in allowed range! "
                      << "Gain = " << gain_ << ". Will reset it to default value!");
      gain_given_ = false;
    }

    if (brightness_given_ && (brightness_ < 0.0 || brightness_ > 255))
    {
      ROS_WARN_STREAM("Desired brightness not in allowed range [0 - 255]! "
                      << "Brightness = " << brightness_ << ". Will reset it to "
                      << "default value!");
      brightness_given_ = false;
    }

    if (exposure_search_timeout_ < 5.)
    {
      ROS_WARN_STREAM("Low timeout for exposure search detected! Exposure "
                      << "search may fail.");
    }
    return;
  }

  const std::string &ArenaCameraParameter::deviceUserID() const
  {
    return device_user_id_;
  }

  std::string ArenaCameraParameter::shutterModeString() const
  {
    if (shutter_mode_ == SM_ROLLING)
    {
      return "rolling";
    }
    else if (shutter_mode_ == SM_GLOBAL)
    {
      return "global";
    }
    else if (shutter_mode_ == SM_GLOBAL_RESET_RELEASE)
    {
      return "global_reset";
    }
    else
    {
      return "default_shutter_mode";
    }
  }

  const std::string &ArenaCameraParameter::imageEncoding() const
  {
    return image_encoding_;
  }

  const std::string &ArenaCameraParameter::cameraFrame() const
  {
    return camera_frame_;
  }

  const double &ArenaCameraParameter::frameRate() const
  {
    return frame_rate_;
  }

  void ArenaCameraParameter::setFrameRate(const ros::NodeHandle &nh, const double &frame_rate)
  {
    frame_rate_ = frame_rate;
    nh.setParam("frame_rate", frame_rate_);
  }

  const std::string &ArenaCameraParameter::cameraInfoURL() const
  {
    return camera_info_url_;
  }

  void ArenaCameraParameter::setCameraInfoURL(const ros::NodeHandle &nh, const std::string &camera_info_url)
  {
    camera_info_url_ = camera_info_url;
    nh.setParam("camera_info_url", camera_info_url_);
  }

  const bool &ArenaCameraParameter::ptpEnable() const
  {
    return ptp_enable_;
  }

  const std::string &ArenaCameraParameter::triggerMode() const
  {
    return trigger_mode_;
  }

  const std::string &ArenaCameraParameter::triggerSource() const
  {
    return trigger_source_;
  }

  const std::string &ArenaCameraParameter::triggerSelector() const
  {
    return trigger_selector_;
  }

} // namespace arena_camera
