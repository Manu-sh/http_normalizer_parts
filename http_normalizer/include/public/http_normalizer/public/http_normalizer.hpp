#pragma once
#include <string>
#include <string_view>
#include <memory>

class http_normalizer { /* http url parser/normalizer */

	protected:
		http_normalizer(const http_normalizer &) = delete;
		http_normalizer operator=(const http_normalizer &) = delete;

	public:
		explicit http_normalizer(const std::string &u);
		static std::shared_ptr<const std::string> normalize(const std::string &u) noexcept;

		/* accessors */
		bool is_https() const { return m_is_https; }
		std::shared_ptr<const std::string> proto()    const { return m_proto;    }
		std::shared_ptr<const std::string> hostname() const { return m_hostname; }
		std::shared_ptr<const std::string> port()     const { return m_port;     }
		std::shared_ptr<const std::string> path()     const { return m_path;     }
		std::shared_ptr<const std::string> query()    const { return m_query;    }
		std::shared_ptr<const std::string> fragment() const { return m_fragment; }

		std::shared_ptr<const std::string> normalized() const;
		std::string dbg_info() const;

	private:
		mutable std::shared_ptr<std::string> m_normalized;
		std::shared_ptr<std::string> m_url;

		std::shared_ptr<std::string> m_proto;
		std::shared_ptr<std::string> m_hostname;
		std::shared_ptr<std::string> m_port;
		std::shared_ptr<std::string> m_path;
		std::shared_ptr<std::string> m_query;
		std::shared_ptr<std::string> m_fragment;
		bool m_is_https = false;
};
