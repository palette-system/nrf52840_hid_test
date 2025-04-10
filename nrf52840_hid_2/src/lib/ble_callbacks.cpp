
#include "Arduino.h"
#include "ble_callbacks.h"



/* ====================================================================================================================== */
/** HID RAW コールバック用 クラス */
/* ====================================================================================================================== */


// ステップ分受信したか確認
int check_step() {
	int i, r = 0;
	for (i=0; i<8; i++) {
		if (save_step_flag[i]) r++;
	}
	return r;
};

void HidrawCallbackExec(int data_length) {
	int h, i, j, k, l, m, s, o, p, x;
    uint8_t *command_id   = &(remap_buf[0]);
    uint8_t *command_data = &(remap_buf[1]);
	tracktall_pim447_data pim447_data_obj;

	// 設定変更がされていて設定変更以外のコマンドが飛んできたら設定を保存
	if (remap_change_flag && *command_id != 0x05) {
		remap_change_flag = 0;
	}
	
	switch (*command_id) {
		case id_get_keyboard_value: { // 0x02 キーボードの情報を送る
			switch (command_data[0]) {
				case id_uptime: { // 0x01 起動してからどれくらい経ったか
					break;
				}
				case id_layout_options: { // 0x02 レイアウトオプション（？）
					remap_buf[2] = 0x00;
					remap_buf[3] = 0x00;
					remap_buf[4] = 0x00;
					remap_buf[5] = 0x00;
					break;
				}
				case id_switch_matrix_state: { // 0x03 スイッチの状態(入力テスト用)
					for (i=2; i<data_length; i++) remap_buf[i] = 0x00;
					m = 2;
					for (i=0; i<key_input_length; i++) {
						remap_buf[m] |= (common_cls.input_key[i])? 1: 0 << (i % 8);
						if ((i %8) == 7) m++;
					}
					remap_input_test = 50;
					break;
				}
			}
			break;
		}
		case id_dynamic_keymap_set_keycode: { // 0x05 設定した内容を保存
			// m = (remap_buf[1] * key_max * 2) + (remap_buf[3] * 2);
			// setting_remap[m] = remap_buf[4];
			// setting_remap[m + 1] = remap_buf[5];
			remap_change_flag = 1;
			break;
		}
		case id_dynamic_keymap_reset: { // 0x06 キーマップリセット とりあえず何もしない
			break;
		}
		case id_dynamic_keymap_macro_get_buffer_size: { // 0x0D マクロキー用のバッファサイズ
			// uint16_t size   = dynamic_keymap_macro_get_buffer_size();
			m = 0;
			remap_buf[1] = (m >> 8) & 0xFF;
            remap_buf[2] = m & 0xFF;
			break;
		}
        case id_dynamic_keymap_macro_get_count: { // 0x0C マクロキーを登録できる数
            remap_buf[1] = 0x00; // dynamic_keymap_macro_get_count();
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: { // 0x0E マクロキーデータを送る
            // uint16_t offset = (command_data[0] << 8) | command_data[1];
            // uint16_t size   = command_data[2];  // size <= 28
            // dynamic_keymap_macro_get_buffer(offset, size, &command_data[3]);
            remap_buf[4] = 0x00;
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: { // 0x0F マクロキーデータを保存
            // uint16_t offset = (command_data[0] << 8) | command_data[1];
            // uint16_t size   = command_data[2];  // size <= 28
            // dynamic_keymap_macro_set_buffer(offset, size, &command_data[3]);
            break;
        }
		case id_dynamic_keymap_get_layer_count: { // 0x11 レイヤー数を送る
			// remap_buf[1] = layer_max;
			remap_buf[1] = 1;
			break;
		}
		case id_dynamic_keymap_get_buffer: { // 0x12 設定データを送る(レイヤー×ROW×COL×2)
			uint16_t r_offset = (command_data[0] << 8) | command_data[1];
			uint16_t r_size   = command_data[2];  // size <= 28
			for (i=0; i<r_size; i++) {
				// remap_buf[4 + i] = setting_remap[r_offset + i];
				remap_buf[4 + i] = 0x00;
			}
			break;
		}


		case id_get_file_start: { // 0x30 ファイル取得開始
			// ファイル名を取得
			i = 1;
			while (remap_buf[i]) {
				target_file_path[i - 1] = remap_buf[i];
				i++;
				if (i >= 32) break;
			}
			target_file_path[i - 1] = 0x00;

		    // ファイルが無ければ0を返す
			if (!InternalFS.exists(target_file_path)) {
				send_buf[0] = id_get_file_start;
				for (i=1; i<32; i++) send_buf[i] = 0x00;
				return;
			}
            File fp = InternalFS.open(target_file_path, FILE_O_READ);
			save_file_length = fp.size();
			// Serial.printf("ps_malloc load: %s %d %d\n", target_file_path, save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
			save_file_data = (uint8_t *)malloc(save_file_length);
			fp.read(save_file_data, save_file_length);
			fp.close();
			send_buf[0] = id_get_file_start;
			send_buf[1] = 0x01; // ファイルは存在する
			send_buf[2] = ((save_file_length >> 24) & 0xff);
			send_buf[3] = ((save_file_length >> 16) & 0xff);
			send_buf[4] = ((save_file_length >> 8) & 0xff);
			send_buf[5] = (save_file_length & 0xff);
			for (i=6; i<32; i++) send_buf[i] = 0x00;
			return;
		    
		}
		case id_get_file_data: { // 0x31 ファイルデータ要求
			return;

		}
		case id_save_file_start: { // 0x32 ファイル保存開始
		    // 容量を取得
			save_file_length = (remap_buf[1] << 24) + (remap_buf[2] << 16) + (remap_buf[3] << 8) + remap_buf[4];
			// 保存時のステップ数
			save_file_step = remap_buf[5];
			// ファイル名を取得
			i = 6;
			while (remap_buf[i]) {
				target_file_path[i - 6] = remap_buf[i];
				i++;
				if (i >= 32) break;
			}
			target_file_path[i - 6] = 0x00;
			// データ受け取りバッファクリア
			// for (i=0; i<512; i++) save_file_data[i] = 0x00;
			// 取得したステップのインデックス
			for (i=0; i<8; i++) save_step_flag[i] = false;
			// ファイルオープン
			// Serial.printf("ps_malloc save: %d %d\n", save_file_length, heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
			save_file_data = (uint8_t *)malloc(save_file_length);
			// open_file = SPIFFS.open(target_file_path, "w");
			// データ要求コマンド送信
			send_buf[0] = id_save_file_data;
			send_buf[1] = save_file_step;
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;

		}
		case id_save_file_data: { // 0x33 ファイルデータ受け取り
		    s = remap_buf[1]; // 何ステップ目のデータか
			j = (remap_buf[2] << 16) + (remap_buf[3] << 8) + remap_buf[4]; // 何処開始のデータか
			m = data_length - 5; // データの長さ
			// バッファにデータを貯める
			p = s * m; // バッファの書込み開始位置
			// Serial.printf("save: %d %d %d\n", j, p, m);
			for (i=0; i<m; i++) {
				// save_file_data[p + i] = remap_buf[i + 5];
				if ((j + p + i) >= save_file_length) break;
				save_file_data[j + p + i] = remap_buf[i + 5];
			}
			// ステップのインデックス加算
			save_step_flag[s] = true;
			// 全ステップ取得した
			if (check_step() >= save_file_step) {
				// ステップインデックスをリセット
				for (i=0; i<8; i++) save_step_flag[i] = false;
				// バッファに入ったデータをファイルに書き出し
				l = m * save_file_step; // 書込みを行うサイズ
				k = j + l; // 書込み後のシークポイント
				// 書込みサイズが保存予定のサイズを超えたら超えない数値にする
				if (k > save_file_length) {
					l = save_file_length - j;
					k = j + l;
				}
				// 書き込む
				h = azcrc32(&save_file_data[j], l);
				if (k < save_file_length) {
					// まだデータを全部受け取って無ければ次を要求するコマンドを送信
					send_buf[0] = id_save_file_data;
					send_buf[1] = save_file_step;
					send_buf[2] = (k >> 24) & 0xff; // データの開始位置 1
					send_buf[3] = (k >> 16) & 0xff; // データの開始位置 2
					send_buf[4] = (k >> 8) & 0xff;  // データの開始位置 3
					send_buf[5] = k & 0xff;         // データの開始位置 4
					send_buf[6] = (h >> 24) & 0xff; // データ確認用ハッシュ 1
					send_buf[7] = (h >> 16) & 0xff; // データ確認用ハッシュ 2
					send_buf[8] = (h >> 8) & 0xff;  // データ確認用ハッシュ 3
					send_buf[9] = h & 0xff;         // データ確認用ハッシュ 4
					for (i=10; i<32; i++) send_buf[i] = 0x00;
					// this->sendRawData(send_buf, 32);
					return;
				} else {
					// データを全部受け取り終わり
					// 完了を送る
					h = azcrc32(save_file_data, save_file_length);
					send_buf[0] = id_save_file_complate; // 保存完了
					send_buf[1] = 0x00; // データ受信完了
					send_buf[2] = (h >> 24) & 0xff; // データ確認用ハッシュ 1
					send_buf[3] = (h >> 16) & 0xff; // データ確認用ハッシュ 2
					send_buf[4] = (h >> 8) & 0xff;  // データ確認用ハッシュ 3
					send_buf[5] = h & 0xff;         // データ確認用ハッシュ 4
					for (i=6; i<32; i++) send_buf[i] = 0x00;
					// this->sendRawData(send_buf, 32);
					return;
				}


			}
			send_buf[0] = 0;
			return;
		}
		case id_save_file_complate: {
			// 保存完了
            common_cls.write_file(target_file_path, save_file_data, save_file_length);
			free(save_file_data);
			send_buf[0] = id_save_file_complate; // 保存完了
			send_buf[1] = 0x01; // データ保存完了
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_remove_file: { // 0x35 ファイル削除要求
			// ファイル名を取得
			i = 1;
			while (remap_buf[i]) {
				target_file_path[i - 1] = remap_buf[i];
				i++;
				if (i >= 32) break;
			}
			target_file_path[i - 1] = 0x00;
			send_buf[0] = id_remove_file;
		    // ファイルがあればファイルを消す
			if (InternalFS.exists(target_file_path)) {
				if (!InternalFS.remove(target_file_path)) {
					// 削除失敗したら2にする
					send_buf[1] = 0x02;
				} else {
					// 成功は0
					send_buf[1] = 0x00;
				}
			} else {
				// ファイルが無ければ1
				send_buf[1] = 0x01;
			}
			// 完了を返す
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;

		}
		case id_remove_all: { // 全てのファイルを削除する
			keyboard_status = 2; // ステータスをAZTOOLの処理中にする
            InternalFS.format();
			keyboard_status = 1; // ステータスを元に戻す

            // 結果を返すコマンドを送信
			send_buf[0] = id_remove_all;
			for (i=1; i<32; i++) send_buf[i] = 0x00;
			return;

		}
		case id_move_file: { // 0x36 ファイル名変更
			// ファイル名を取得
			i = 1;
			j = 0;
			while (remap_buf[i]) {
				target_file_path[j] = remap_buf[i];
				i++;
				j++;
				if (i >= 32) break;
			}
			target_file_path[j] = 0x00;
			i++;
			// 変更後ファイル名を取得
			j = 0;
			while (remap_buf[i]) {
				second_file_path[j] = remap_buf[i];
				i++;
				j++;
				if (i >= 32) break;
			}
			second_file_path[j] = 0x00;
		    send_buf[0] = id_move_file;
			if (!InternalFS.exists(target_file_path)) {
				// 該当ファイルが無ければ1を返す
				send_buf[1] = 0x01;
			} else if (InternalFS.rename(target_file_path, second_file_path)) {
				// ファイル名変更 成功
				send_buf[1] = 0x00;
			} else {
				// ファイル名変更 失敗
				send_buf[1] = 0x02;
			}
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;

        }
		case id_get_file_list: {
			// 0x38 ファイルリストの取得
            File dirp = InternalFS.open("/", FILE_O_READ);
			File filep = dirp.openNextFile();
			String res = "{\"list\":[";
			i = 0;
			while(filep){
				if (i) res += ",";
				res += "{\"name\":\"" +String(filep.name()) + "\",\"size\":" + String(filep.size()) + "}";
				filep = dirp.openNextFile();
				i++;
			}
			res += "]}";
			save_file_length = res.length();
			m = save_file_length;
			// ファイルリストの結果を送信用バッファに入れる
			save_file_data = (uint8_t *)malloc(m + 1);
			res.toCharArray((char *)save_file_data, m + 1);
			// 結果を返すコマンドを送信
			send_buf[0] = id_get_file_list;
			send_buf[1] = ((save_file_length >> 24) & 0xff);
			send_buf[2] = ((save_file_length >> 16) & 0xff);
			send_buf[3] = ((save_file_length >> 8) & 0xff);
			send_buf[4] = (save_file_length & 0xff);
			for (i=5; i<32; i++) send_buf[i] = 0x00;
			// this->sendRawData(send_buf, 32);
			return;
		}
		case id_get_disk_info: {
			// 0x39 SPIFFSの容量を返す
			send_buf[0] = id_get_disk_info; // 結果の返すコマンド
			// spiffs の容量
			m = 0;
			send_buf[1] = ((m >> 24) & 0xff);
			send_buf[2] = ((m >> 16) & 0xff);
			send_buf[3] = ((m >> 8) & 0xff);
			send_buf[4] = (m & 0xff);
			// spiffs の使用容量
			m = 0;
			send_buf[5] = ((m >> 24) & 0xff);
			send_buf[6] = ((m >> 16) & 0xff);
			send_buf[7] = ((m >> 8) & 0xff);
			send_buf[8] = (m & 0xff);
			// 結果を返すコマンドを送信
			for (i=9; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_restart: {
			// 0x3a の再起動
			aztool_mode_flag = 3; // キーボードリスタート要求
			send_buf[0] = id_restart; // 結果の返すコマンド
			for (i=1; i<32; i++) send_buf[i] = 0x00;
			return;

		}
		case id_get_ioxp_key: {
			// IOエキスパンダからキーの読み取り
			uint8_t rows[8]; // rowのピン
			uint16_t out_mask; // rowのピンを立てたマスク
			bool sa, sb;
			x = remap_buf[1]; // エキスパンダのアドレス(0～7)
			// 既に使用しているIOエキスパンダなら読み込みステータス0で返す
			if (ioxp_hash[x] == 1) {
				send_buf[0] = id_get_ioxp_key; // IOエキスパンダキー読み込み
				send_buf[1] = 0x01; // 使用中
				for (i=2; i<32; i++) send_buf[i] = 0x00;
				// this->sendRawData(send_buf, 32);
				return;
			}
			// 初期化がまだであれば初期化
			if (ioxp_status[x] < 0) {
                ioxp_obj[x] = new Adafruit_MCP23X17();
                ioxp_status[x] = 0;
			}
			if (ioxp_status[x] < 1) {
				if (!ioxp_obj[x]->begin_I2C(0x20 + x, &Wire)) {
					// 初期化失敗
					send_buf[0] = id_get_ioxp_key; // IOエキスパンダキー読み込み
					send_buf[1] = 0x02; // 初期化失敗
					for (i=2; i<32; i++) send_buf[i] = 0x00;
					// this->sendRawData(send_buf, 32);
					return;
				}
                ioxp_status[x] = 1;
			}
			// row の情報を取得
			s = remap_buf[2]; // row の数
			if (s > 8) s = 8;
			for (i=0; i<s; i++) {
				rows[i] = remap_buf[3 + i]; // row の番号取得
			}
			// ピンの初期化
			h = azcrc32(remap_buf, 32); // 受け取ったデータのハッシュを取得
			if (h != ioxp_hash[x]) { // 最後に設定したピン情報と違えばピンの初期化をする
				for (i=0; i<16; i++) {
					// row のピンかチェックして rowならOUTPUTに指定
					k = false;
					for (j=0; j<s; j++) {
						if (rows[j] == i) {
							ioxp_obj[x]->pinMode(i, OUTPUT);
							k = true;
							break;
						}
					}
					if (k) continue; // row だったなら次のピンへ
					// row以外は全てinput
					ioxp_obj[x]->pinMode(i, INPUT_PULLUP);
				}
				ioxp_hash[x] = h;
			}
			// キーの読み込み
			p = 3;
			send_buf[0] = id_get_ioxp_key; // IOエキスパンダキー読み込み
			send_buf[1] = 0x00; // 読み取り成功
			send_buf[2] = s; // rowの数
			if (s) {
				// row があればマトリックス読み取り
				// マスク作成
				out_mask = 0x00;
				for (i=0; i<s; i++) {
					out_mask |= (0x01 << rows[i]);
				}
				// マトリックス読み込み
				for (i=0; i<s; i++) {
					o = out_mask & ~(0x01 << rows[i]);
					if (out_mask & 0xff00) { // ポートB
						ioxp_obj[x]->writeGPIO((o >> 8) & 0xff, 1); // ポートBに出力
					}
					if (out_mask & 0xff) { // ポートA
						ioxp_obj[x]->writeGPIO(o & 0xff, 0); // ポートAに出力
					}
					h = ~(ioxp_obj[x]->readGPIOAB() | out_mask); // ポートA,B両方のデータを取得(rowのピンは全て1)
					send_buf[p] = (h >> 8) & 0xff;
					p++;
					send_buf[p] = h & 0xff;
					p++;
				}
			} else {
				// row が無ければ全ピンダイレクト
				h = ~(ioxp_obj[x]->readGPIOAB()); // ポートA,B両方のデータを取得
				send_buf[p] = (h >> 8) & 0xff;
				p++;
				send_buf[p] = h & 0xff;
				p++;
			}
			// 結果を送信
			for (i=p; i<32; i++) send_buf[i] = 0x00; // 残りのデータを0詰め
			// this->sendRawData(send_buf, 32);
			return;
		}
		case id_set_mode_flag: {
			// WEBツール作業中フラグの設定
			aztool_mode_flag = remap_buf[1];
			// 結果を送信
			send_buf[0] = id_set_mode_flag; // フラグの設定
			for (i=1; i<32; i++) send_buf[i] = 0x00; // 残りのデータを0詰め
			// this->sendRawData(send_buf, 32);
			return;
			
		}
		case id_get_ap_list: {
			// WIFI のアクセスポイントの一覧取得
			send_buf[0] = id_get_ap_list;
			for (i=1; i<32; i++) send_buf[i] = 0x00;
			return;

		}
		case id_read_key: {
			// キーの入力状態取得
			// 結果コマンドの準備
			send_buf[0] = id_read_key; // キーの入力状態
			send_buf[1] = key_input_length & 0xff; // キーの数
			for (i=2; i<32; i++) send_buf[i] = 0x00; // 残りのデータを0詰め
			// 結果コマンドに入力データを入れていく
			j = 0;
			s = 2;
			for (i=0; i<key_input_length; i++) {
				if (j == 8) {
					j = 0;
					s++;
				} else {
					send_buf[s] = send_buf[s] << 1;
				}
				if (s > 31) break;
				if (common_cls.input_key[i]) send_buf[s]++;
				j++;
			}
			if (j > 0 && j < 8) send_buf[s] = send_buf[s] << (8 - j);
			// 結果を送信
			// this->sendRawData(send_buf, 32);
			return;

		}
		case id_get_rotary_key: {
			// ロータリーエンコーダの入力状態取得
		    m = remap_buf[1]; // 読み込みに行くアドレス取得
			send_buf[0] = id_get_rotary_key; // キーの入力状態
			send_buf[1] = m; // 読み込みに行くアドレス
            send_buf[2] = wirelib_cls.read_rotary(m); // データ受け取る
			for (i=3; i<32; i++) send_buf[i] = 0x00;
			// this->sendRawData(send_buf, 32);
			return;

		}
		case id_get_pim447: {
			// 1U トラックボールデータ取得 PIM447
		    m = remap_buf[1]; // 読み込みに行くアドレス取得
			send_buf[0] = id_get_pim447; // キーの入力状態
			send_buf[1] = m; // 読み込みに行くアドレス
            pim447_data_obj = wirelib_cls.read_trackball_pim447(m); // データ受け取る
			send_buf[2] = pim447_data_obj.left;
			send_buf[3] = pim447_data_obj.right;
			send_buf[4] = pim447_data_obj.up;
			send_buf[5] = pim447_data_obj.down;
			send_buf[6] = pim447_data_obj.click;
			for (i=7; i<32; i++) send_buf[i] = 0x00;
			// this->sendRawData(send_buf, 32);
			return;

		}
		case id_set_pin_set: {
			// ESP32 本体の direct, touch, col, row を変更する(pinModeの初期化もする)
			// 現状でi2c の設定がされている場合は一旦無しにしちゃう
			// (i2c で使ってるピンがioで設定されるとややこしい話になる)
			if (ioxp_sda >= 0 && ioxp_scl >= 0) {
				// i2c 通信終了
				// Wire.end(); // ボード ESP32 の 1.0.6 の Wire に end() が無かった
				// ピンの設定削除
				ioxp_sda = -1;
				ioxp_scl = -1;
			}
			if (i2copt_len > 0) {
				i2copt_len = -1;
				delete[] i2copt;
			}
			// 現状のピン設定を解放
			delete[] direct_list;
			delete[] touch_list;
			delete[] col_list;
			delete[] row_list;
			// 読み込み開始バイト
			m = 1;
			// direct ピン設定
			direct_len = remap_buf[m]; // direct ピンの設定数取得
			m++;
			direct_list = new short[direct_len];
			for (i=0; i<direct_len; i++) {
				direct_list[i] = remap_buf[m];
				m++;
			}
			// touch ピン設定
			touch_len = remap_buf[m]; // touch ピンの設定数取得
			m++;
			touch_list = new short[touch_len];
			for (i=0; i<touch_len; i++) {
				touch_list[i] = remap_buf[m];
				m++;
			}
			// col ピン設定
			col_len = remap_buf[m]; // col ピンの設定数取得
			m++;
			col_list = new short[col_len];
			for (i=0; i<col_len; i++) {
				col_list[i] = remap_buf[m];
				m++;
			}
			// row ピン設定
			row_len = remap_buf[m]; // row ピンの設定数取得
			m++;
			row_list = new short[row_len];
			for (i=0; i<row_len; i++) {
				row_list[i] = remap_buf[m];
				m++;
			}
			// キー数計算
			common_cls.pin_setup();
			// レスポンスデータ作成
			send_buf[0] = id_set_pin_set; // ピン設定
			send_buf[1] = ((key_input_length >> 24) & 0xff);
			send_buf[2] = ((key_input_length >> 16) & 0xff);
			send_buf[3] = ((key_input_length >> 8) & 0xff);
			send_buf[4] = (key_input_length & 0xff);
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_i2c_read: {
			// i2c からデータ読み込み
		    m = remap_buf[1]; // 読み込みに行くアドレス取得
			l = remap_buf[2]; // 読み込む長さ
			if (l > 28) l = 28; // 読み込む長さは28バイトがMAX
			for (i=0; i<32; i++) send_buf[i] = 0x00;
			send_buf[0] = id_i2c_read; // キーの入力状態
			send_buf[1] = m; // 読み込みに行くアドレス
			send_buf[2] = wirelib_cls.read(m, &send_buf[3], l); // 読み込み
			return;
		}
		case id_i2c_write: {
			// i2c へデータ書込み
		    m = remap_buf[1]; // 書込みに行くアドレス取得
			l = remap_buf[2]; // 書き込む長さ
			if (l > 28) l = 28; // 書き込む長さは28バイトがMAX
			send_buf[0] = id_i2c_write; // キーの入力状態
			send_buf[1] = m; // 読み込みに行くアドレス
			send_buf[2] = wirelib_cls.write(m, &remap_buf[3], l); // 書込み
			for (i=2; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_get_analog_switch: {
			// アナログスイッチの情報を取得
			send_buf[0] = id_get_analog_switch;
			for (i=1; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_set_analog_switch: {
			// アナログスイッチの設定を変更
			send_buf[0] = id_set_analog_switch;
			for (i=1; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_get_serial_input: {
			// シリアル通信(赤外線)のキー入力取得
			send_buf[0] = id_get_serial_input;
			p = 1;
			for (i=0; i<256; i++) {
				s = i / 16;
				x = i % 16;
				if (seri_input[s] & (0x01 << x)) {
					send_buf[p] = i;
					p++;
				}
				if (p >=32) break;
			}
			for (i=p; i<32; i++) send_buf[i] = 0x00;
			return;
		}
		case id_get_serial_setting: {
			// シリアル通信(赤外線)のセッティング情報取得
			send_buf[0] = id_get_serial_setting;
			for (i=1; i<(sizeof(seri_setting)+1); i++) {
				send_buf[i] = seri_setting[i-1];
			}
			while (i<32) { send_buf[i] = 0x00; i++; }
			return;
		}
		case id_get_firmware_status: {
			// ファームウェアステータス取得
			sprintf((char *)send_buf, "%c%s-%s", id_get_firmware_status, FIRMWARE_VERSION, EEP_DATA_VERSION);
			// this->sendRawData(send_buf, 32);
			return;

		}

		default: {
			send_buf[0] = 0xFF;
			for (i=1; i<32; i++) send_buf[i] = 0x00;
            return;

        }
	}
}
