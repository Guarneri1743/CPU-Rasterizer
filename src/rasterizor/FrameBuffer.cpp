#include "FrameBuffer.hpp"
#include <execution>
#include <algorithm>

namespace Guarneri
{
	FrameBuffer::FrameBuffer(const size_t& w, const size_t& h, const FrameContent& flag) : 
		content_flag(flag),
		width(w), height(h)
	{
		resize(w, h);
	}

	bool FrameBuffer::perform_stencil_test(const uint8_t& ref_val,
										   const uint8_t& read_mask, 
										   const CompareFunc& func,
										   const size_t& row, 
										   const size_t& col) const
	{
		if ((content_flag & FrameContent::kStencil) == FrameContent::kNone)
		{
			return false;
		}

		bool pass = false;
		uint8_t stencil;
		if (stencil_buffer->read(row, col, stencil))
		{
			switch (func)
			{
			case CompareFunc::kNever:
				pass = false;
				break;
			case CompareFunc::kAlways:
				pass = true;
				break;
			case CompareFunc::kEqual:
				pass = (ref_val & read_mask) == (stencil & read_mask);
				break;
			case CompareFunc::kGreater:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::kLEqual:
				pass = (ref_val & read_mask) <= (stencil & read_mask);
				break;
			case CompareFunc::kNotEqual:
				pass = (ref_val & read_mask) != (stencil & read_mask);
				break;
			case CompareFunc::kGEqual:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::kLess:
				pass = (ref_val & read_mask) < (stencil & read_mask);
				break;
			}
		}
		return pass;
	}

	void FrameBuffer::update_stencil_buffer(const size_t& row, 
											const size_t& col,
											const PerSampleOperation& op_pass,
											const StencilOp& stencil_pass_op, 
											const StencilOp& stencil_fail_op, 
											const StencilOp& stencil_zfail_op, 
											const uint8_t& ref_val) const
	{
		if ((content_flag & FrameContent::kStencil) == FrameContent::kNone)
		{
			return;
		}

		bool stencil_pass = (op_pass & PerSampleOperation::kStencilTest) != PerSampleOperation::kNone;
		bool z_pass = (op_pass & PerSampleOperation::kDepthTest) != PerSampleOperation::kNone;
		uint8_t stencil;
		stencil_buffer->read(row, col, stencil);
		StencilOp stencil_op;
		if (stencil_pass)
		{
			stencil_op = z_pass ? stencil_pass_op : stencil_zfail_op;
		}
		else
		{
			stencil_op = stencil_fail_op;
		}
		switch (stencil_op)
		{
		case StencilOp::kKeep:
			break;
		case StencilOp::kZero:
			stencil_buffer->write(row, col, 0);
			break;
		case StencilOp::kReplace:
			stencil_buffer->write(row, col, ref_val);
			break;
		case StencilOp::kIncrement:
			stencil_buffer->write(row, col, std::clamp((uint8_t)(stencil + 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::kDecrement:
			stencil_buffer->write(row, col, std::clamp((uint8_t)(stencil - 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::kIncrementWrap:
			stencil_buffer->write(row, col, stencil + 1);
			break;
		case StencilOp::kDecrementWrap:
			stencil_buffer->write(row, col, stencil - 1);
			break;
		case StencilOp::kInvert:
			stencil_buffer->write(row, col, ~stencil);
			break;
		}
	}

	bool FrameBuffer::perform_depth_test(const CompareFunc& func,
										 const size_t& row, 
										 const size_t& col, 
										 const float& z) const
	{
		if ((content_flag & FrameContent::kDepth) == FrameContent::kNone)
		{
			return false;
		}

		float depth;
		bool pass = false;
		if (depth_buffer->read(row, col, depth))
		{
			pass = z <= depth;
			switch (func)
			{
			case CompareFunc::kNever:
				pass = false;
				break;
			case CompareFunc::kAlways:
				pass = true;
				break;
			case CompareFunc::kEqual:
				pass = tinymath::approx(z, depth);
				break;
			case CompareFunc::kGreater:
				pass = z > depth;
				break;
			case CompareFunc::kLEqual:
				pass = z <= depth;
				break;
			case CompareFunc::kNotEqual:
				pass = z != depth;
				break;
			case CompareFunc::kGEqual:
				pass = z >= depth;
				break;
			case CompareFunc::kLess:
				pass = z < depth;
				break;
			}
		}
		return pass;
	}

	// todo: alpha factor
	tinymath::Color FrameBuffer::blend(const tinymath::Color& src_color,
									   const tinymath::Color& dst_color,
									   const BlendFactor& src_factor, 
									   const BlendFactor& dst_factor, 
									   const BlendOp& op)
	{
		tinymath::Color lhs, rhs;
		switch (src_factor)
		{
		case BlendFactor::kOne:
			lhs = src_color;
			break;
		case BlendFactor::kSrcAlpha:
			lhs = src_color * src_color.a;
			break;
		case BlendFactor::kSrcColor:
			lhs = src_color * src_color;
			break;
		case BlendFactor::kOneMinusSrcAlpha:
			lhs = src_color * (1.0f - src_color);
			break;
		case BlendFactor::kOneMinusSrcColor:
			lhs = src_color * (1.0f - dst_color);
			break;
		case BlendFactor::kDstAlpha:
			lhs = src_color * dst_color.a;
			break;
		case BlendFactor::kDstColor:
			lhs = src_color * dst_color;
			break;
		case BlendFactor::kOneMinusDstAlpha:
			lhs = src_color * (1.0f - dst_color.a);
			break;
		case BlendFactor::kOneMinusDstColor:
			lhs = src_color * (1.0f - dst_color);
			break;
		}

		switch (dst_factor)
		{
		case BlendFactor::kOne:
			rhs = src_color;
			break;
		case BlendFactor::kSrcAlpha:
			rhs = dst_color * src_color.a;
			break;
		case BlendFactor::kSrcColor:
			rhs = dst_color * src_color;
			break;
		case BlendFactor::kOneMinusSrcAlpha:
			rhs = dst_color * (1.0f - src_color);
			break;
		case BlendFactor::kOneMinusSrcColor:
			rhs = dst_color * (1.0f - dst_color);
			break;
		case BlendFactor::kDstAlpha:
			rhs = dst_color * dst_color.a;
			break;
		case BlendFactor::kDstColor:
			rhs = dst_color * dst_color;
			break;
		case BlendFactor::kOneMinusDstAlpha:
			rhs = dst_color * (1.0f - dst_color.a);
			break;
		case BlendFactor::kOneMinusDstColor:
			rhs = dst_color * (1.0f - dst_color);
			break;
		}

		switch (op)
		{
		case BlendOp::kAdd:
			return lhs + rhs;
		case BlendOp::kSub:
			return lhs - rhs;
		}
		return lhs + rhs;
	}

	void FrameBuffer::write_color(const size_t& row, const size_t& col, const tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::kColor) != FrameContent::kNone)
		{
			color_buffer->write(row, col, color);
		}
	}

	void FrameBuffer::write_depth(const size_t & row, const size_t & col, const float& depth)
	{
		if ((content_flag & FrameContent::kDepth) != FrameContent::kNone)
		{
			depth_buffer->write(row, col, depth);
		}
	}

	void FrameBuffer::write_stencil(const size_t & row, const size_t & col, const uint8_t & stencil)
	{
		if ((content_flag & FrameContent::kStencil) != FrameContent::kNone)
		{
			stencil_buffer->write(row, col, stencil);
		}
	}

	void FrameBuffer::write_coverage(const size_t & row, const size_t & col, const uint8_t & coverage)
	{
		if ((content_flag & FrameContent::kCoverage) != FrameContent::kNone)
		{
			coverage_buffer->write(row, col, coverage);
		}
	}

	bool FrameBuffer::read_color(const size_t & row, const size_t & col, tinymath::color_rgba & color)
	{
		if ((content_flag & FrameContent::kColor) == FrameContent::kNone)
		{
			return false;
		}

		return color_buffer->read(row, col, color);
	}

	bool FrameBuffer::read_depth(const size_t& row, const size_t& col, float& depth)
	{
		if ((content_flag & FrameContent::kDepth) == FrameContent::kNone)
		{
			return false;
		}

		return depth_buffer->read(row, col, depth);
	}

	bool FrameBuffer::read_stencil(const size_t& row, const size_t& col, uint8_t& stencil)
	{
		if ((content_flag & FrameContent::kStencil) == FrameContent::kNone)
		{
			return false;
		}

		return stencil_buffer->read(row, col, stencil);
	}

	bool FrameBuffer::read_coverage(const size_t& row, const size_t& col, uint8_t& coverage)
	{
		if ((content_flag & FrameContent::kCoverage) == FrameContent::kNone)
		{
			return false;
		}

		return coverage_buffer->read(row, col, coverage);
	}



	void FrameBuffer::write_color(const float& u, const float& v, const tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::kColor) != FrameContent::kNone)
		{
			color_buffer->write(u, v, color);
		}
	}

	void FrameBuffer::write_depth(const float& u, const float& v, const float& depth)
	{
		if ((content_flag & FrameContent::kDepth) != FrameContent::kNone)
		{
			depth_buffer->write(u, v, depth);
		}
	}

	void FrameBuffer::write_stencil(const float& u, const float& v, const uint8_t& stencil)
	{
		if ((content_flag & FrameContent::kStencil) != FrameContent::kNone)
		{
			stencil_buffer->write(u, v, stencil);
		}
	}

	void FrameBuffer::write_coverage(const float& u, const float& v, const uint8_t& coverage)
	{
		if ((content_flag & FrameContent::kCoverage) != FrameContent::kNone)
		{
			coverage_buffer->write(u, v, coverage);
		}
	}

	bool FrameBuffer::read_color(const float& u, const float& v, tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::kColor) == FrameContent::kNone)
		{
			return false;
		}

		return color_buffer->read(u, v, color);
	}

	bool FrameBuffer::read_depth(const float& u, const float& v, float& depth)
	{
		if ((content_flag & FrameContent::kDepth) == FrameContent::kNone)
		{
			return false;
		}

		return depth_buffer->read(u, v, depth);
	}

	bool FrameBuffer::read_stencil(const float& u, const float& v, uint8_t& stencil)
	{
		if ((content_flag & FrameContent::kStencil) == FrameContent::kNone)
		{
			return false;
		}

		return stencil_buffer->read(u, v, stencil);
	}

	bool FrameBuffer::read_coverage(const float& u, const float& v, uint8_t& coverage)
	{
		if ((content_flag & FrameContent::kCoverage) == FrameContent::kNone)
		{
			return false;
		}

		return coverage_buffer->read(u, v, coverage);
	}

	void FrameBuffer::clear(const FrameContent& flag)
	{
		if ((flag & content_flag & FrameContent::kColor) != FrameContent::kNone)
		{
			color_buffer->clear(ColorEncoding::encode_rgba(tinymath::kColorBlack));
		}

		if ((flag & content_flag & FrameContent::kDepth) != FrameContent::kNone)
		{
			depth_buffer->clear(kFarZ);
		}

		if ((flag & content_flag & FrameContent::kStencil) != FrameContent::kNone)
		{
			stencil_buffer->clear(kDefaultStencil);
		}

		if ((flag & content_flag & FrameContent::kCoverage) != FrameContent::kNone)
		{
			coverage_buffer->clear((uint8_t)0);
		}
	}

	void FrameBuffer::resize(const size_t& w, const size_t& h)
	{
		this->width = w;
		this->height = h;

		if ((content_flag & FrameContent::kColor) != FrameContent::kNone)
		{
			if (color_buffer == nullptr)
			{
				color_buffer = std::make_unique<RawBuffer<tinymath::color_rgba>>(w, h);
			}
			else
			{
				color_buffer->resize(w, h);
			}
		}

		if ((content_flag & FrameContent::kDepth) != FrameContent::kNone)
		{
			if (depth_buffer == nullptr)
			{
				depth_buffer = std::make_unique<RawBuffer<float>>(w, h);
			}
			else
			{
				depth_buffer->resize(w, h);
			}
		}

		if ((content_flag & FrameContent::kStencil) != FrameContent::kNone)
		{
			if (stencil_buffer == nullptr)
			{
				stencil_buffer = std::make_unique<RawBuffer<uint8_t>>(w, h);
			}
			else
			{
				stencil_buffer->resize(w, h);
			}
		}

		if ((content_flag & FrameContent::kCoverage) != FrameContent::kNone)
		{
			if (coverage_buffer == nullptr)
			{
				coverage_buffer = std::make_unique<RawBuffer<uint8_t>>(w, h);
			}
			else
			{
				coverage_buffer->resize(w, h);
			}
		}
	}
}
