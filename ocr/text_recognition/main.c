/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stdint.h>
#include <stdio.h>
#include <crt_config.h>
#include <tvm_runtime.h>
#include <tvmgen_text_recognition.h>
#include "inputs.h"
#include "outputs.h"
#include "stdout_USART.h"


int main(int argc, char** argv) {
  char dict[]={"#0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~!\"#$%&'()*+,-./  "};
  int char_dict_nums = 97;
  stdout_init();
  printf("Starting ocr rec inference\n");
  struct tvmgen_text_recognition_outputs rec_outputs = {
      .output = output,
  };
  struct tvmgen_text_recognition_inputs rec_inputs = {
      .x = input,
  };

  tvmgen_text_recognition_run(&rec_inputs, &rec_outputs);

  // post process
  int char_nums = output_len / char_dict_nums;

  int last_index = 0;
  float score = 0.f;
  int count = 0;

  printf("text: ");
  for (int i = 0; i < char_nums; i++) {
    int argmax_idx = 0;
    float max_value = 0.0f;
    for (int j = 0; j < char_dict_nums; j++){
      if (output[i * char_dict_nums + j] > max_value){
        max_value = output[i * char_dict_nums + j];
        argmax_idx = j;
      }
    }
    if (argmax_idx > 0 && (!(i > 0 && argmax_idx == last_index))) {
      score += max_value;
      count += 1;
      // printf("%d，%f，%c\n", argmax_idx, max_value, dict[argmax_idx]);
      printf("%c", dict[argmax_idx]);
    }
    last_index = argmax_idx;
  }
  score /= count;
  printf(", score: %f\n", score);

  // The FVP will shut down when it receives "EXITTHESIM" on the UART
  printf("EXITTHESIM\n");
  while (1 == 1);
  return 0;
}