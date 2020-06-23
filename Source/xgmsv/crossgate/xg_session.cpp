#pragma once

#include "core/shared.h"

#include "core/network/timer.h"

#include "crossgate/xg_session.h"
#include "crossgate/xg_packet.h"

namespace server
{
	namespace crossgate
	{
		struct xg_session_pool xg_session_pool;

		xg_session::xg_session(core::tcp_connection* connection, connection_type type) :
			connections{}
		{
			connections[(uint32)type] = connection;

			connection->rqueue = &rqueue;
			connection->wqueue = &wqueue;
			connection->wsignal = &signal;
		}

		xg_session_pool::xg_session_pool() :
			session_count(0)
		{}

		xg_session* xg_session_pool::spawn_session(core::tcp_connection* connection, connection_type type)
		{
			for (int id = 0; id < this->size(); id++)
			{
				auto& new_session = this->at(id);
				if (!new_session)
				{
					this->session_count++;
					new_session = new xg_session(connection, type);
					new_session->id = id;
					return new_session;
				}
			}

			return nullptr;
		}

		void init_user_session_pool(uint32 max_session_count)
		{
			core::async_every(1000ms, []()
			{

				int sessions_purged = 0;
				for (auto& session : xg_session_pool)
				{
					if (session)
					{
						uint32 id = session->id;

						auto inactive_seconds = std::chrono::duration_cast<std::chrono::seconds>(core::server_time - session->last_active);

						if (inactive_seconds.count() > 60)
						{
							delete session;
							session = nullptr;
							xg_session_pool.session_count--;
							sessions_purged++;
						}

						//if (session->marked_for_delete)
						//{
						//}
					}
				}

				if (sessions_purged)
				{
					printf("%d sessions purged.\n", sessions_purged);
				}

			});

			xg_session_pool.resize(max_session_count);
		}


		void xg_session::handle_echo(xg_packet* packet)
		{
			if (!send_msg(R"(Echo nr )"))
			{
				printf("handle_echo failed.\n");
			}

			printf("handle_echo!\n");
		}


		void xg_session::handle_fc(xg_packet* packet)
		{
			printf("handle_fc!\n");
		}

		void xg_session::handle_client_login(xg_packet* packet)
		{
			enum status_code
			{
				success = 0,
				wrong_login_or_banned = 1,
				refuse = 2
			};

			if (!send_msg(R"(ClientLogin 0 block )"))
			{
				printf("handle_client_login failed.\n");
			}

			printf("handle_client_login!\n");
		}

		void xg_session::handle_char_list(xg_packet* packet)
		{
			enum fields
			{
				name = 0,
				something = 1,
				portrait = 2,
				level = 3,
				vitality = 4,
				strength = 5,
				toughness = 6,
				quickness = 7,
				magic = 8,
				fire = 9,
				wind = 10,
				earth = 11,
				water = 12,
				login_times = 13,
				name_again = 14,
				soemthing2 = 15,
				char_class = 16,
				something3 = 17,
				char_sprite = 18,
				something4 = 19
			};

			send_msg(R"(PV 8 )");
			send_msg(R"(CharList 0 jason|0\\z241401\\z1\\z15\\z0\\z0\\z0\\z15\\z0\\z0\\z100\\z0\\z12\\zjason\\z2\\z��ϰ����ʿ\\z2\\z106002\\z-1| )");

			printf("handle_char_list!\n");
		}

		void xg_session::handle_char_create(xg_packet* packet)
		{
			//"CreateNewChar 1 ϡ��֥ʿ rDW 10ZE 0 0 0 f f 0 0 5 5 "
			//"CreateNewChar successful 3"
		}

		void xg_session::handle_char_delete(xg_packet* packet)
		{
			//"CharDelete 2 " left
			//"CharDelete 3 " right
			//"CharDelete successful 3" ? not sure
		}

		void xg_session::handle_char_login(xg_packet* packet)
		{
			send_msg(R"(PRV 3|5|100|0 )");

			send_msg(R"(CharLogin successful  )");
			
			send_msg(R"(LI 3H9 1JbdtH 1 0 )");
			
			send_msg(R"(CC 0 hV p J c 6 0 0 0 0 -1 )");
			
			send_msg(R"(MN ����ҽԺ\\z0 )");
			
			send_msg(R"(CP 1|155|155|185|185|15|0|0|0|15|50|50|50|0|16|1|23|23|23|107|107|60|100|100|0|0|0|5000|0|jason|| )");
			
			send_msg(R"(CP2 1|0|0|0|0|0|0|0|0|0|0|100|0|0|0|0|241400|10|100| )");
			
			send_msg(R"(CJ 1 ��ϰ����ʿ )");
			
			//send_msg(R"(CS 0|||||||||||1|||||||||||2|||||||||||3|||||||||||4|||||||||||5|||||||||||6|||||||||||7|||||||||||8|||||||||||9|||||||||||10|||||||||||11|||||||||||12|||||||||||13|||||||||||14||||||||||| )");
			
			//send_msg(R"(TITLE ��η�ļž�|0|17|��ϰ����ʿ|1|161|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| )");
			
			//send_msg(R"(I 7|����ˮ��|0|$4�ȼ�\S1\\n$4�;�\S1000/1000\S$0���\Sˮ��\\n\\n\\n\\n\\n\\n\\n$4����װ����ˮ��\\n\\n$1�ŵ��ϱ����ʧ\S\\n$1�޷��ó����ʼ�����\S\\n|27511|1|2|0|1|184|9200|22|0||8|մ����ˮ����|0|$4�ȼ�\S1\\n$0���\Sδ֪\\n\\n\\n\\n\\n\\n\\n$4\\n\\n$1�ŵ��ϱ����ʧ\S\\n$1�޷��ó����ʼ�����\S\\n|26579|1|0|0|1|56|22238|26|0||9|ʱ��ˮ��Lv6|0|$4�ȼ�\S1\\n$4�;�\S0001/0001\S$0���\Sδ֪\\n�ָܻ���ҵʱ���ˮ�����ָ����Сʱ����ˮ���ȼ���ȡ�\\n\\n\\n\\n\\n$4�ָܻ���ҵʱ���ˮ �����ָ����Сʱ����ˮ���ȼ���ȡ�\\n\\n$1�ŵ��ϱ����ʧ\S\\n$1�޷��ó����ʼ�����\S\\n|99123|1|0|0|1|56|22693|26|1||10| ���⹤����|0|$4�ȼ�\S1\\n$4�;�\S0005/0005\S$0���\Sδ֪\\n\\n\\n\\n\\n\\n\\n$4\\n\\n|27487|1|0|0|1|25|22022|26|0||11|�����ظ�ҩˮ(200)|0|$4�ȼ�\S3\\n$0���\SҩƷ\\n�ָ�Ŀ������ֵԼ200��\\n\\n\\n\\n\\n\\n$4�ظ�������200���ҩ\\n\\n|26218|1|1|93|3|25|15608|43|20||12|������ë-��ҽ|0|$4�ȼ�\S1\\n$4�;�\S0004/0005\S$0���\Sδ֪\\n\\n\\n\\n\\n\\n\\n$4˲��ʹ��͵���ҽԺ��������ë��\\n\\n|27828|1|0|0|1|25|18779|26|0||13|ɮ�����Լ��ϸ�֤|0|$4�ȼ�\S1\\n$0���\Sδ֪\\n\\n\\n\\n\\n\\n\\n$4����ʿְҵ�ľ�ְ�Ƽ���\\n\\n$1�ŵ��ϱ����ʧ\S\\n$1�޷��ó����ʼ�����\S\\n|27881|0|0|0|1|56|18106|26|0||14|���������|0|$4�ȼ�\S1\\n$0���\Sδ֪\\n\\n\\n\\n\\n\\n\\n$4��������������\S\S\\n\\n$1�ŵ��ϱ����ʧ\S\\n$1�޷��ó����ʼ�����\S\\n|26573|0|0|72|1|56|18786|26|0| )");
			
			send_msg(R"(EP 0 0 )");
			
			//send_msg(R"(KP 0 1|101321|4|93|93|99|99|1|16|1|48|40|34|101|100|24|0|40|60|0|10|1|���ֺ����||0|0| )");
			
			//send_msg(R"(KP2 0 1|2|9|4|5|3|25|0|45|-10|0|0|0|0|0|0|1| )");
			
			//send_msg(R"(PT 0 0|7300|����|������ͨ������������|0|1|1141|0|1|7400|����|�ܷ��������������Ĵ��|0|1|72|1|2|407|������\SLV8|�������ǰ����λ��һ���������˺������ȼ��ı乥������|40|1|117|2|3|1238|����ֹˮ-��|���о���ظ�һ���������������� �ܺ��������ֵԽ�߻ظ���������Խ��(ע��ͻظ����޹�)��ʹ�ú��޷�������������\S|135|1|72|3| )");
			
			send_msg(R"(FS 0 )");
			
			send_msg(R"(MC 0 hV 0 0 p k bFX agh 0 )");
			
			//send_msg(R"(AB ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| )");
			
			//send_msg(R"(ABG ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| )");
			
			//send_raw(R"(PBt-ztsrSCXizpHiNuKPg5AuyS6PzdstfC7blf3rb-Dbo7tf5NhhQ-+Pg4--7kE6qjGnMfl6th1N20udvQ)");
			send_msg(R"(C 1|3H9|12|6|0|106003|1|0|jason|\\z\\z|0|0|0|��ϰ����ʿ|0 )");
			
			send_msg(R"(C 2|1tq|10|5|4|14088|1|0|����ҽʦ|\\z\\z|0|0|0||0	37|1ty|10|17|4|14088|1|0|ķ��ҽʦ|\\z\\z|0|0|0||0	2|1ti|12|5|4|14090|1|0|ʵϰҩ��ʦ����|\\z\\z|0|0|0||0	29|1xH|16|9|6|14151|1|0|ʵϰ������|\\z\\z|0|0|0||0 )");
			
			send_msg(R"(BT 5Co 0 )");
			send_msg(R"(POS 0 )");
			send_msg(R"(AL 1 0|0|0|0|0|0|0|0|0|0| )");
			send_msg(R"(IP 127.0.0.1 )");
			send_msg(R"(MAC Y O )");
			send_msg(R"(EF 0 0  )");
			send_msg(R"(TK -1 P|��ӭ����֥ʿ��ħ������������~ 4 0 )");
			send_msg(R"(CA 3H9|12|6|0|0 )");

			/*
			send_raw(R"(TK -1 P|��л��������ħ����wow335.taobao.com 4 0 )");
			send_raw(R"(TK -1 P|[�汾����]GMSV\SAvaritia\SFeb\S\S1\S2014\S����� 4 0 )");
			send_raw(R"(TK -1 P|������˽����о�ʹ�ã�����������ҵ��;�� 4 0 )");
			send_raw(R"(TK -1 P|��Ŀ��ҳ\S&\S������̳��http://www.cgdev.me/ 4 0 )");
			send_raw(R"(STK GA\SLua�������������� )");
			send_raw(R"(STK [����ħ������]����\S/help\Sָ��鿴��ǰ����LuaTalkָ�� )");
			*/
			//send_raw(R"()");
			/*
			*/
		}

		void xg_session::handle_char_logout(xg_packet* packet)
		{
			send_msg(R"(CharLogout successful )");
			printf("handle_char_logout!\n");

			//signal.fire(core::signal_code::shutdown);

			//marked_for_delete = true;
		}

		void xg_session::handle_char_walk(xg_packet* packet)
		{

		}

		void xg_session::handle_char_interact(xg_packet* packet)
		{

		}

	}
}
