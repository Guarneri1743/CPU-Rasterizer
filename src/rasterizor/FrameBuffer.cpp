#include "FrameBuffer.hpp"

namespace Guarneri
{
	FrameBuffer::FrameBuffer(const size_t& w, const size_t& h, const FrameContent& flag)
	{
		this->content_flag = flag;
		resize(w, h);
	}

	bool FrameBuffer::perform_stencil_test(const uint8_t& ref_val, const uint8_t& read_mask, const CompareFunc& func, const size_t& row, const size_t& col) const
	{
		if ((content_flag & FrameContent::Stencil) == FrameContent::None)
		{
			return false;
		}

		bool pass = false;
		uint8_t stencil;
		if (stencil_buffer->read(row, col, stencil))
		{
			switch (func)
			{
			case CompareFunc::NEVER:
				pass = false;
				break;
			case CompareFunc::ALWAYS:
				pass = true;
				break;
			case CompareFunc::EQUAL:
				pass = (ref_val & read_mask) == (stencil & read_mask);
				break;
			case CompareFunc::GREATER:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::LEQUAL:
				pass = (ref_val & read_mask) <= (stencil & read_mask);
				break;
			case CompareFunc::NOT_EQUAL:
				pass = (ref_val & read_mask) != (stencil & read_mask);
				break;
			case CompareFunc::GEQUAL:
				pass = (ref_val & read_mask) > (stencil & read_mask);
				break;
			case CompareFunc::LESS:
				pass = (ref_val & read_mask) < (stencil & read_mask);
				break;
			}
		}
		return pass;
	}

	void FrameBuffer::update_stencil_buffer(const size_t& row, const size_t& col, const PerSampleOperation& op_pass, const StencilOp& stencil_pass_op, const StencilOp& stencil_fail_op, const StencilOp& stencil_zfail_op, const uint8_t& ref_val) const
	{
		if ((content_flag & FrameContent::Stencil) == FrameContent::None)
		{
			return;
		}

		bool stencil_pass = (op_pass & PerSampleOperation::STENCIL_TEST) != PerSampleOperation::DISABLE;
		bool z_pass = (op_pass & PerSampleOperation::DEPTH_TEST) != PerSampleOperation::DISABLE;
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
		case StencilOp::KEEP:
			break;
		case StencilOp::ZERO:
			stencil_buffer->write(row, col, 0);
			break;
		case StencilOp::REPLACE:
			stencil_buffer->write(row, col, ref_val);
			break;
		case StencilOp::INCR:
			stencil_buffer->write(row, col, std::clamp((uint8_t)(stencil + 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::DECR:
			stencil_buffer->write(row, col, std::clamp((uint8_t)(stencil - 1), (uint8_t)0, (uint8_t)255));
			break;
		case StencilOp::INCR_WRAP:
			stencil_buffer->write(row, col, stencil + 1);
			break;
		case StencilOp::DECR_WRAP:
			stencil_buffer->write(row, col, stencil - 1);
			break;
		case StencilOp::INVERT:
			stencil_buffer->write(row, col, ~stencil);
			break;
		}
	}

	bool FrameBuffer::perform_depth_test(const CompareFunc& func, const size_t& row, const size_t& col, const float& z) const
	{
		if ((content_flag & FrameContent::Depth) == FrameContent::None)
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
			case CompareFunc::NEVER:
				pass = false;
				break;
			case CompareFunc::ALWAYS:
				pass = true;
				break;
			case CompareFunc::EQUAL:
				pass = tinymath::approx(z, depth);
				break;
			case CompareFunc::GREATER:
				pass = z > depth;
				break;
			case CompareFunc::LEQUAL:
				pass = z <= depth;
				break;
			case CompareFunc::NOT_EQUAL:
				pass = z != depth;
				break;
			case CompareFunc::GEQUAL:
				pass = z >= depth;
				break;
			case CompareFunc::LESS:
				pass = z < depth;
				break;
			}
		}
		return pass;
	}

	void FrameBuffer::write_color(const size_t& row, const size_t& col, const tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::Color) != FrameContent::None)
		{
			color_buffer->write(row, col, color);
		}
	}

	void FrameBuffer::write_depth(const size_t & row, const size_t & col, const float& depth)
	{
		if ((content_flag & FrameContent::Depth) != FrameContent::None)
		{
			depth_buffer->write(row, col, depth);
		}
	}

	void FrameBuffer::write_stencil(const size_t & row, const size_t & col, const uint8_t & stencil)
	{
		if ((content_flag & FrameContent::Stencil) != FrameContent::None)
		{
			stencil_buffer->write(row, col, stencil);
		}
	}

	void FrameBuffer::write_coverage(const size_t & row, const size_t & col, const uint8_t & coverage)
	{
		if ((content_flag & FrameContent::Coverage) != FrameContent::None)
		{
			coverage_buffer->write(row, col, coverage);
		}
	}

	bool FrameBuffer::read_color(const size_t & row, const size_t & col, tinymath::color_rgba & color)
	{
		if ((content_flag & FrameContent::Color) == FrameContent::None)
		{
			return false;
		}

		return color_buffer->read(row, col, color);
	}

	bool FrameBuffer::read_depth(const size_t& row, const size_t& col, float& depth)
	{
		if ((content_flag & FrameContent::Depth) == FrameContent::None)
		{
			return false;
		}

		return depth_buffer->read(row, col, depth);
	}

	bool FrameBuffer::read_stencil(const size_t& row, const size_t& col, uint8_t& stencil)
	{
		if ((content_flag & FrameContent::Stencil) == FrameContent::None)
		{
			return false;
		}

		return stencil_buffer->read(row, col, stencil);
	}

	bool FrameBuffer::read_coverage(const size_t& row, const size_t& col, uint8_t& coverage)
	{
		if ((content_flag & FrameContent::Coverage) == FrameContent::None)
		{
			return false;
		}

		return coverage_buffer->read(row, col, coverage);
	}



	void FrameBuffer::write_color(const float& u, const float& v, const tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::Color) != FrameContent::None)
		{
			color_buffer->write(u, v, color);
		}
	}

	void FrameBuffer::write_depth(const float& u, const float& v, const float& depth)
	{
		if ((content_flag & FrameContent::Depth) != FrameContent::None)
		{
			depth_buffer->write(u, v, depth);
		}
	}

	void FrameBuffer::write_stencil(const float& u, const float& v, const uint8_t& stencil)
	{
		if ((content_flag & FrameContent::Stencil) != FrameContent::None)
		{
			stencil_buffer->write(u, v, stencil);
		}
	}

	void FrameBuffer::write_coverage(const float& u, const float& v, const uint8_t& coverage)
	{
		if ((content_flag & FrameContent::Coverage) != FrameContent::None)
		{
			coverage_buffer->write(u, v, coverage);
		}
	}

	bool FrameBuffer::read_color(const float& u, const float& v, tinymath::color_rgba& color)
	{
		if ((content_flag & FrameContent::Color) == FrameContent::None)
		{
			return false;
		}

		return color_buffer->read(u, v, color);
	}

	bool FrameBuffer::read_depth(const float& u, const float& v, float& depth)
	{
		if ((content_flag & FrameContent::Depth) == FrameContent::None)
		{
			return false;
		}

		return depth_buffer->read(u, v, depth);
	}

	bool FrameBuffer::read_stencil(const float& u, const float& v, uint8_t& stencil)
	{
		if ((content_flag & FrameContent::Stencil) == FrameContent::None)
		{
			return false;
		}

		return stencil_buffer->read(u, v, stencil);
	}

	bool FrameBuffer::read_coverage(const float& u, const float& v, uint8_t& coverage)
	{
		if ((content_flag & FrameContent::Coverage) == FrameContent::None)
		{
			return false;
		}

		return coverage_buffer->read(u, v, coverage);
	}



	void FrameBuffer::clear(const FrameContent& flag)
	{
		if ((flag & content_flag & FrameContent::Color) != FrameContent::None)
		{
			color_buffer->clear(ColorEncoding::encode_rgba(tinymath::kColorBlack));
		}

		if ((flag & content_flag & FrameContent::Depth) != FrameContent::None)
		{
			depth_buffer->clear(kFarZ);
		}

		if ((flag & content_flag & FrameContent::Stencil) != FrameContent::None)
		{
			stencil_buffer->clear(kDefaultStencil);
		}

		if ((flag & content_flag & FrameContent::Coverage) != FrameContent::None)
		{
			coverage_buffer->clear(0);
		}
	}

	void FrameBuffer::resize(const size_t& w, const size_t& h)
	{
		this->width = w;
		this->height = h;

		if ((content_flag & FrameContent::Color) != FrameContent::None)
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

		if ((content_flag & FrameContent::Depth) != FrameContent::None)
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

		if ((content_flag & FrameContent::Stencil) != FrameContent::None)
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

		if ((content_flag & FrameContent::Coverage) != FrameContent::None)
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
